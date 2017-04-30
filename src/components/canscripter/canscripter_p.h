#ifndef CANSCRIPTER_P_H
#define CANSCRIPTER_P_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <cstdint>
#include <map>
#include <tuple>
#include <vector>
#include <QTimer>
#include <memory>

namespace {
const QString cJsonNameTag = "name";
const QString cJsonValueTag = "value";
const QString cJsonPreDelayTag = "pre-delay";
const QString cJsonPostDelayTag = "post-delay";
const QString cJsonValueStartTag = "value-start";
const QString cJsonValueStopTag = "value-stop";
const QString cJsonValueStepTag = "value-step";
const QString cJsonDurationTag = "duration";
}

struct TimerStep {
    QString name{ "" };
    uint32_t value{ 0 };
    uint32_t delay{ 0 };
};

class CanScripterTimer : public QTimer
{
    Q_OBJECT

public:
    CanScripterTimer(const std::vector<TimerStep> &steps) : 
        QTimer(nullptr),
        timerSteps(steps)
    {
        connect(this, &CanScripterTimer::timeout, this, &CanScripterTimer::step);
        setSingleShot(true);
    }

Q_SIGNALS:
    void sendSignal(const QString& name, const QByteArray& value);

private Q_SLOTS:
    void step()
    {
       qDebug() << "Step! " << timerSteps[ndx].name;
       if(timerSteps[ndx].name.length() > 0) {
           emit sendSignal(timerSteps[ndx].name, QByteArray::number(timerSteps[ndx].value));
       }

       setInterval(timerSteps[ndx].delay);
       ndx++;
       
       if(ndx < timerSteps.size()) {
           start();
       }
    }

private:
    const std::vector<TimerStep> timerSteps;
    uint32_t ndx {0};
};


class CanScripterPrivate : public QObject {
    Q_OBJECT

public:
    CanScripterPrivate(CanScripter *q) :
        q_ptr(q)
    {
    }

    bool parseScriptObject(const QJsonObject& obj)
    {
        for(auto &timer : timers) {
            timer->stop();
        }

        timers.clear();

        for (const auto& timer : obj) {
            if (timer.isArray()) {
                auto &&timerSteps = processTimerSteps(timer.toArray());

                if (timerSteps.empty()) {
                    return false;
                } else {
                    auto &&timer = std::make_shared<CanScripterTimer>(timerSteps);
                    connect(timer.get(), &CanScripterTimer::sendSignal, q_ptr, &CanScripter::sendSignal);
                    timer->setInterval(0);
                    timers.push_back(timer);
                }

                qDebug() << "\n***************** TIMER " << timers.size() << " *****************\n";
                for (auto& el : timerSteps) {
                    qDebug() << "name: " << el.name << ", val: " << el.value << ", delay: " << el.delay;
                }

            } else {
                qDebug() << "ERROR: Timer should contain Array!";
                qDebug() << timer;
                return false;
            }
        }

        return true;
    }

    void start()
    {
        for(auto &timer : timers) {
            timer->start();
        }
    }

    void stop()
    {
        for(auto &timer : timers) {
            timer->stop();
        }

        timers.clear();
    }

    QString scriptName;

private:
    inline bool isMultiStep(const QJsonObject& obj)
    {
        return (tagExists(obj, cJsonNameTag) && tagExists(obj, cJsonValueStartTag) && tagExists(obj, cJsonValueStopTag) && tagExists(obj, cJsonValueStepTag) && tagExists(obj, cJsonDurationTag));
    }

    inline bool isSingleStep(const QJsonObject& obj)
    {
        return (tagExists(obj, cJsonNameTag) && tagExists(obj, cJsonValueTag));
    }

    inline bool isDelayStep(const QJsonObject& obj)
    {
        return (tagExists(obj, cJsonPreDelayTag) || tagExists(obj, cJsonPostDelayTag));
    }

    inline bool tagExists(const QJsonObject& obj, const QString& tag)
    {
        return !obj[tag].isNull() && !obj[tag].isUndefined();
    }

    std::vector<TimerStep> processTimerSteps(const QJsonArray timer)
    {
        std::vector<TimerStep> timerSteps;

        for (const auto& timerStep : timer) {
            if (timerStep.isObject()) {
                const auto& stepObj = timerStep.toObject();

                if (isMultiStep(stepObj)) {
                    QString name = stepObj[cJsonNameTag].toString();
                    uint32_t valueStart = stepObj[cJsonValueStartTag].toInt();
                    uint32_t valueStop = stepObj[cJsonValueStopTag].toInt();
                    uint32_t valueStep = stepObj[cJsonValueStepTag].toInt();
                    uint32_t duration = stepObj[cJsonDurationTag].toInt();
                    uint32_t stepDuration = duration / (valueStop - valueStart) / valueStep;
                    uint32_t preDelay = stepObj[cJsonPreDelayTag].toInt();
                    uint32_t postDelay = stepObj[cJsonPostDelayTag].toInt();
                    uint32_t val;

                    if(preDelay) {
                        timerSteps.push_back({ "", 0, preDelay });
                    }

                    for (val = valueStart; val <= valueStop; val += valueStep) {
                        timerSteps.push_back({ name, val, stepDuration });
                    }

                    if (val < valueStop) {
                        timerSteps.push_back({ name, valueStop, stepDuration });
                    }

                    // set delay after last step to postDelay
                    timerSteps.back().delay = postDelay;

                } else if (isSingleStep(stepObj)) {
                    QString name = stepObj[cJsonNameTag].toString();
                    uint32_t value = stepObj[cJsonValueTag].toInt();
                    uint32_t preDelay = stepObj[cJsonPreDelayTag].toInt();
                    uint32_t postDelay = stepObj[cJsonPostDelayTag].toInt();

                    if(preDelay) {
                        timerSteps.push_back({ "", 0, preDelay });
                    }
                    
                    timerSteps.push_back({ name, value, postDelay });

                } else if (isDelayStep(stepObj)) {
                    uint32_t preDelay = stepObj[cJsonPreDelayTag].toInt();
                    uint32_t postDelay = stepObj[cJsonPostDelayTag].toInt();

                    timerSteps.push_back({ "", 0, preDelay + postDelay });

                } else {
                    qDebug() << "ERROR: unkown step type";
                    qDebug() << "ERROR: " << stepObj;
                    timerSteps.clear();
                    break;
                }

            } else {
                qDebug() << "ERROR: TimerStep should contain object!";
                qDebug() << "ERROR: " << timerStep;
                timerSteps.clear();
                break;
            }
        }

        return timerSteps;
    }

    std::vector<std::shared_ptr<CanScripterTimer>> timers;
    CanScripter *q_ptr;
};

#endif // CANSCRIPTER_P_H
