// Implementation for Problem 059 - OJ 的继承与派生
#include <cstring>
#include <cmath>
#include <algorithm>
#include <string>

class BaseJudger {
public:
    BaseJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : time_limit_(time_limit), memory_limit_(memory_limit), score_(0) {
        if (answer) {
            size_t n = std::strlen(answer);
            answer_ = new char[n + 1];
            std::memcpy(answer_, answer, n + 1);
        } else {
            answer_ = new char[1];
            answer_[0] = '\0';
        }
    }

    virtual void Submit(size_t time, size_t memory, const char *output) = 0;

    size_t GetScore() const { return score_; }

    virtual ~BaseJudger() {
        delete[] answer_;
    }

protected:
    char *answer_;
    const size_t time_limit_;
    const size_t memory_limit_;
    size_t score_;

    virtual bool CheckAnswer(const char *output) const {
        // the output must equal to the answer
        return std::strcmp(answer_, output) == 0;
    }
};

class OIJudger : public BaseJudger {
public:
    OIJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer), submitted_(false), invalid_(false) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (invalid_) {
            // already invalid due to multiple submissions
            score_ = 0;
            return;
        }
        if (submitted_) {
            // more than one submission -> directly zero
            invalid_ = true;
            score_ = 0;
            return;
        }
        submitted_ = true;

        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            score_ = 100;
        } else {
            score_ = 0;
        }
    }

protected:
    // Compare answer and output by trimming trailing spaces for each line
    bool CheckAnswer(const char *output) const override {
        auto normalize = [](const char *s) -> std::string {
            std::string res;
            const char *p = s;
            std::string line;
            while (*p) {
                if (*p == '\n') {
                    // trim trailing spaces
                    size_t end = line.size();
                    while (end > 0 && line[end - 1] == ' ') {
                        --end;
                    }
                    res.append(line.c_str(), end);
                    res.push_back('\n');
                    line.clear();
                } else {
                    line.push_back(*p);
                }
                ++p;
            }
            // If the last line does not end with '\n', still compare it
            if (!line.empty()) {
                size_t end = line.size();
                while (end > 0 && line[end - 1] == ' ') {
                    --end;
                }
                res.append(line.c_str(), end);
            }
            return res;
        };

        std::string a = normalize(answer_);
        std::string b = normalize(output);
        return a == b;
    }

private:
    bool submitted_;
    bool invalid_;
};

class ICPCJudger : public BaseJudger {
public:
    ICPCJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        size_t cand = 0;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            cand = 100;
        }
        if (cand > score_) score_ = cand;
    }
};

class SpacialJudger : public BaseJudger {
public:
    SpacialJudger(size_t time_limit, size_t memory_limit,
                  size_t full_score_time, size_t full_score_memory,
                  const char *answer)
        : BaseJudger(time_limit, memory_limit, answer),
          full_score_time_(full_score_time), full_score_memory_(full_score_memory) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        size_t cand = 0;
        if (CheckAnswer(output)) {
            int tscore = score_component(time, full_score_time_, time_limit_);
            int mscore = score_component(memory, full_score_memory_, memory_limit_);
            if (tscore <= 0 || mscore <= 0) {
                cand = 0;
            } else {
                cand = static_cast<size_t>(std::floor((tscore * mscore) / 100.0));
            }
        }
        if (cand > score_) score_ = cand;
    }

private:
    size_t full_score_time_;
    size_t full_score_memory_;

    static int score_component(size_t val, size_t full, size_t limit) {
        if (val <= full) return 100;
        if (val >= limit) return 0;
        if (limit <= full) {
            // Degenerate case: treat as step at limit
            return (val <= limit) ? 100 : 0;
        }
        // Linear mapping: full -> 100, limit -> 0
        // floor( (limit - val) * 100 / (limit - full) )
        size_t numerator = (limit - val) * 100;
        size_t denom = (limit - full);
        return static_cast<int>(numerator / denom);
    }
};

