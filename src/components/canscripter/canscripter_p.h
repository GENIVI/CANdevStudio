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
    uint32_t preDelay{ 0 };
    uint32_t postDelay{ 0 };
};

class CanScripterPrivate {
public:
    bool parseScriptObject(const QJsonObject& obj)
    {
        for (const auto& timer : obj) {
            if (timer.isArray()) {
                auto timerSteps = processTimerSteps(timer.toArray());

                if (timerSteps.empty()) {
                    return false;
                } else {
                    timersActions.push_back(timerSteps);
                }
            } else {
                qDebug() << "ERROR: Timer should contain Array!";
                qDebug() << timer;
                return false;
            }
        }

        return true;
    }

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
                    uint32_t val;

                    for (val = valueStart; val <= valueStop; val += valueStep) {
                        timerSteps.push_back({ name, val, 0, stepDuration });
                    }

                    if (val < valueStop) {
                        timerSteps.push_back({ name, valueStop, 0, stepDuration });
                    }

                } else if (isSingleStep(stepObj)) {
                    QString name = stepObj[cJsonNameTag].toString();
                    uint32_t value = stepObj[cJsonValueTag].toInt();
                    uint32_t preDelay = stepObj[cJsonPreDelayTag].toInt();
                    uint32_t postDelay = stepObj[cJsonPostDelayTag].toInt();

                    if (preDelay && postDelay) {
                        timerSteps.push_back({ name, value, preDelay, 0 });
                        timerSteps.push_back({ "", value, 0, postDelay });
                    } else {
                        timerSteps.push_back({ name, value, preDelay, postDelay });
                    }

                } else if (isDelayStep(stepObj)) {
                    uint32_t preDelay = stepObj[cJsonPreDelayTag].toInt();
                    uint32_t postDelay = stepObj[cJsonPostDelayTag].toInt();

                    timerSteps.push_back({ "", 0, preDelay + postDelay, 0 });

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

public:
    std::vector<std::vector<TimerStep> > timersActions;
    QString scriptName;
};

#endif // CANSCRIPTER_P_H
