#!/usr/bin/env ruby

require 'net/https'
require 'json'
require 'uri'
require 'securerandom'
require 'pathname'
require 'date'

def get_git_config(setting_name)
	val =`git config #{setting_name}`
	if $?.success?
		val
	else
		nil
	end
end

def get_git_blob_contents(ref, fileName)
    pr_number = ENV["TRAVIS_PULL_REQUEST"]

    if pr_number == "false"
	    contents = `git show #{ref}:#{fileName}`
    else
        # sed is so so evil... but it works ;)
		contents = `git show #{ref}:#{fileName} | sed 's/#&& git fetch origin pull\\/XXX/\\&\\& git fetch origin pull\\/#{pr_number}/g'`
    end

	if $?.success?
		contents
	else
		nil
	end	
end

def parse_commits(lines, separator)
	commits = []
	commit = {
		:author => {}
	}
	num = 1
	message = []
	lines.each { |line|
		case
		when num == 1
			commit[:id] = line
		when num == 2
			commit[:author][:name] = line
		when num == 3
			commit[:author][:email] = line
		when num == 4
			commit[:timestamp] = DateTime.parse(line)
		when num > 4 && line != separator
			message << line
		when line == separator
			commit[:message] = message.join('\n')
			commits << commit

			num = 0
			message = []
			commit = {
				:author => {}
			}
		end
		num += 1
	}
	commits
end

start_commit_id = nil
end_commit_id = nil
ref = nil

# read input data from STDIN in the format "base commit ref"
STDIN.each_line do |line|
  start_commit_id, end_commit_id, ref = line.strip.split
end

# current directory is repository root
repo_path = Dir.pwd
repo_name = Pathname.new(repo_path).basename

#puts "Input data: #{start_commit_id} #{end_commit_id} #{ref}"
#puts "Repository path: #{repo_path}"

# get git config
webhook_url = ENV["APPVEYOR_WEBHOOK"] 
#puts "Webhook URL: #{webhook_url}"
pr_number = ENV["TRAVIS_PULL_REQUEST"]

comments_end = SecureRandom.hex
log_format = "--date=rfc --format=%H%n%an%n%ae%n%ad%n%B%n#{comments_end}"
commits = []

if pr_number != "false"
	result = `git log #{ENV["TRAVIS_PULL_REQUEST_SHA"]} -1 #{log_format}`.split("\n")
	commits += parse_commits(result, comments_end)
    puts commits[0][:message] = "Pull Request ##{pr_number}: #{commits[0][:message]}"
elsif start_commit_id == "0000000000000000000000000000000000000000"
	# tag
	result = `git log #{end_commit_id} -1 #{log_format}`.split("\n")
	commits += parse_commits(result, comments_end)
else
	# get commits exclude start commit
	result = `git log #{start_commit_id}..#{end_commit_id} #{log_format}`.split("\n")
	commits += parse_commits(result, comments_end)

	# get start commit only
	result = `git log #{start_commit_id} -1 #{log_format}`.split("\n")
	commits += parse_commits(result, comments_end)
end

#commits.each { |commit| puts "commit: #{commit[:id]}" }

# get blob contents
appveyor_yml = get_git_blob_contents(end_commit_id, ".appveyor.yml")
puts appveyor_yml

payload = {
	:ref => ref,
	:before => start_commit_id,
	:after => end_commit_id,
	:commits => commits,
	:head_commit => commits.first,
	:repository => {
		:name => repo_name
	},
	:config => appveyor_yml
}

# send webhook
uri = URI(webhook_url)
req = Net::HTTP::Post.new(uri.request_uri, initheader = {'Content-Type' =>'application/json'})
# ruby 2.0: req = Net::HTTP::Post.new uri
#req.basic_auth 'username', 'password'
req.body = payload.to_json

response = Net::HTTP.start(uri.host, uri.port, :use_ssl => uri.scheme == "https") do |http|
    http.verify_mode = OpenSSL::SSL::VERIFY_NONE
    http.request req
end

if response.code != "200" and response.code != "204"
	raise "Error sending webhook: #{response.code}"
end
