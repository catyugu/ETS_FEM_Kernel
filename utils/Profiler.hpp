#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stack>
#include <thread>
#include <mutex>
#include <limits>
#include <algorithm>

namespace Utils {
    class Profiler {
    public:
        static Profiler &instance();
        Profiler(const Profiler &) = delete;
        Profiler &operator=(const Profiler &) = delete;

        void begin(const std::string &name);
        void end();
        std::string getReport() const;
        void reset();
        void setEnabled(bool enabled);
        bool isEnabled() const;

    private:
        Profiler();

        struct ProfileData {
            size_t callCount = 0;
            double totalTime = 0.0;
            double selfTime = 0.0; // 新增：用于记录自身耗时（不包含子调用）
            double minTime = (std::numeric_limits<double>::max)();
            double maxTime = 0.0;
            mutable std::mutex mutex;

            void update(double duration, double children_duration) {
                std::lock_guard<std::mutex> lock(mutex);
                callCount++;
                totalTime += duration;
                selfTime += (duration - children_duration); // 计算自身耗时
                if (duration < minTime) minTime = duration;
                if (duration > maxTime) maxTime = duration;
            }
        };

        struct ProfileRecord {
            std::string name;
            std::chrono::high_resolution_clock::time_point startTime;
            double childrenDuration = 0.0; // 新增：记录所有直接子调用的总耗时
        };

        static thread_local std::stack<ProfileRecord> recordStack_;

        mutable std::unordered_map<std::string, std::unique_ptr<ProfileData>> profileData_;
        mutable std::mutex dataMutex_;

        std::chrono::high_resolution_clock::time_point programStartTime_;
        bool enabled_ = true;
    };

    class ProfileScope {
    public:
        explicit ProfileScope(const std::string &name);
        ~ProfileScope();

    private:
        ProfileScope(const ProfileScope &) = delete;
        ProfileScope &operator=(const ProfileScope &) = delete;
    };
} // namespace Utils

#define PROFILE_FUNCTION() ::Utils::ProfileScope profile_scope_##__LINE__(__FUNCTION__)
#define PROFILE_SCOPE(name) ::Utils::ProfileScope profile_scope_##__LINE__(name)

#endif // PROFILER_HPP