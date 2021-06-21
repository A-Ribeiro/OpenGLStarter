#ifndef Interpolators_h__
#define Interpolators_h__

#include <aribeiro/aribeiro.h>

namespace GLEngine {

    template <class C>
    class _SSE2_ALIGN_PRE Key {
    public:
        float time;
        C value;
        Key() {}
        Key(float _time, const C &_value) {
            time = _time;
            value = _value;
        }
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    template <class T>
    class _SSE2_ALIGN_PRE LinearInterpolator {

        float lastTimeQuery;
        int lastIndexA;
        int lastIndexB;
        T lastReturned;

    public:
        typename aRibeiro::aligned_vector< Key<T> > keys;

        LinearInterpolator() {
            lastTimeQuery = 0.0f;
            lastIndexA = 0;
            lastIndexB = 1;
            lastReturned = T();
        }

        void addKey(const Key<T> &v) {
            keys.push_back(v);
            if (keys.size() == 1) {
                lastTimeQuery = v.time;
                lastReturned = v.value;
            }
        }

        void resetToBegin() {
            if (keys.size() > 0) {
                lastIndexA = 0;
                lastIndexB = 1;
                lastTimeQuery = keys[0].time;
                lastReturned = keys[0].value;
            }
        }

        T getValue_ForwardLoop(float t, T *interquery_delta = NULL) {
            if (keys.size() == 0)
                return T();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            if (t < lastTimeQuery) {
                //add the last animation value related to the last returned
                if (interquery_delta != NULL)
                    *interquery_delta = keys[keys.size() - 1].value - lastReturned;
                resetToBegin();
            }
            else {
                if (interquery_delta != NULL)
                    *interquery_delta = T(0);
            }

            if (t > lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexA < keys.size() - 2; lastIndexA++) {
                    if (t < keys[lastIndexA + 1].time)
                        break;
                }
                lastIndexB = lastIndexA + 1;
            }
            else {
                //if (interquery_delta != NULL)
                //    *interquery_delta = T(0);
                return lastReturned;
            }

            float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[lastIndexA].time) / delta;

            T result = aRibeiro::lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

            if (interquery_delta != NULL)
                *interquery_delta += result - lastReturned;

            lastReturned = result;
            return lastReturned;
        }

        T getValue(float t) {
            if (keys.size() == 0)
                return T();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            if (t > lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexA < keys.size() - 2; lastIndexA++) {
                    if (t < keys[lastIndexA + 1].time)
                        break;
                }
                lastIndexB = lastIndexA + 1;
            }
            else if (t < lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexB > 1; lastIndexB--) {
                    if (t > keys[lastIndexB - 1].time)
                        break;
                }
                lastIndexA = lastIndexB - 1;
            }
            else
                return lastReturned;


            float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[lastIndexA].time) / delta;

            lastReturned = aRibeiro::lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

            return lastReturned;
        }

        T getValue_slow(float t) const {

            if (keys.size() == 0)
                return T();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            int indexA;

            for (indexA = 0; indexA < keys.size() - 1; indexA++) {
                if (t < keys[indexA + 1].time)
                    break;
            }

            if (indexA >= keys.size() - 1)
                return keys[keys.size() - 1].value;

            int indexB = indexA + 1;

            float delta = (keys[indexB].time - keys[indexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[indexA].time) / delta;

            return aRibeiro::lerp(keys[indexA].value, keys[indexB].value, lrp);
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;


    class _SSE2_ALIGN_PRE SlerpInterpolator {

        float lastTimeQuery;
        int lastIndexA;
        int lastIndexB;
        aRibeiro::quat lastReturned;

    public:
        aRibeiro::aligned_vector< Key<aRibeiro::quat> > keys;

        SlerpInterpolator()
        {
            lastTimeQuery = 0.0f;
            lastIndexA = 0;
            lastIndexB = 1;
            lastReturned = aRibeiro::quat();
        }

        void addKey(const Key<aRibeiro::quat> &v) {
            keys.push_back(v);
            if (keys.size() == 1) {
                lastTimeQuery = v.time;
                lastReturned = v.value;
            }
        }

        void resetToBegin() {
            if (keys.size() > 0) {
                lastIndexA = 0;
                lastIndexB = 1;
                lastTimeQuery = keys[0].time;
                lastReturned = keys[0].value;
            }
        }

        aRibeiro::quat getValue_ForwardLoop(float t, aRibeiro::quat *interquery_delta = NULL) {
            if (keys.size() == 0)
                return aRibeiro::quat();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            if (t < lastTimeQuery) {
                //add the last animation value related to the last returned
                if (interquery_delta != NULL)
                    *interquery_delta = inv(lastReturned) * keys[keys.size() - 1].value;
                resetToBegin();
            }
            else {
                if (interquery_delta != NULL)
                    *interquery_delta = aRibeiro::quat();
            }

            if (t > lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexA < keys.size() - 2; lastIndexA++) {
                    if (t < keys[lastIndexA + 1].time)
                        break;
                }
                lastIndexB = lastIndexA + 1;
            }
            else
                return lastReturned;

            float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[lastIndexA].time) / delta;

            aRibeiro::quat result = aRibeiro::slerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

            if (interquery_delta != NULL)
                *interquery_delta = (*interquery_delta) * (inv(lastReturned) * result);

            lastReturned = result;
            return lastReturned;
        }

        aRibeiro::quat getValue(float t) {
            if (keys.size() == 0)
                return aRibeiro::quat();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            if (t > lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexA < keys.size() - 2; lastIndexA++) {
                    if (t < keys[lastIndexA + 1].time)
                        break;
                }
                lastIndexB = lastIndexA + 1;
            }
            else if (t < lastTimeQuery) {
                lastTimeQuery = t;

                for (; lastIndexB > 1; lastIndexB--) {
                    if (t > keys[lastIndexB - 1].time)
                        break;
                }
                lastIndexA = lastIndexB - 1;
            }
            else
                return lastReturned;


            float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[lastIndexA].time) / delta;

            lastReturned = aRibeiro::slerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

            return lastReturned;
        }

        aRibeiro::quat getValue_slow(float t) const {

            if (keys.size() == 0)
                return aRibeiro::quat();
            else if (keys.size() == 1)
                return keys[0].value;

            t = aRibeiro::clamp(t, keys[0].time, keys[keys.size() - 1].time);

            int indexA;

            for (indexA = 0; indexA < keys.size() - 1; indexA++) {
                if (t < keys[indexA + 1].time)
                    break;
            }

            if (indexA >= keys.size() - 1)
                return keys[keys.size() - 1].value;

            int indexB = indexA + 1;

            float delta = (keys[indexB].time - keys[indexA].time);
            float lrp;

            if (delta <= aRibeiro::EPSILON)
                lrp = 0.0f;
            else
                lrp = (t - keys[indexA].time) / delta;

            return aRibeiro::slerp(keys[indexA].value, keys[indexB].value, lrp);
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;
}

#endif