/** @file
 * @ingroup matching
 *
 * @defgroup multi_matcher
 * Class to hold multiple instances of a matcher and multithread matches
 */

#ifndef WAVE_MULTI_MATCHER_HPP
#define WAVE_MULTI_MATCHER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <tuple>
#include "wave/utils/math.hpp"
#include "wave/matching/pcl_common.hpp"
#include "wave/matching/matcher.hpp"

namespace wave {
/** @addtogroup matching
 *  @{ */

/**
 * Class is templated for different matcher types
 * @tparam T matcher type
 * @tparam R matcher params type
 */
template <typename T, typename R>
class MultiMatcher {
 public:
    MultiMatcher(int n_threads = std::thread::hardware_concurrency(),
                 int queue_s = 10,
                 R params = R())
        : n_thread(n_threads), queue_size(queue_s), config(params) {
        this->stop = false;
        this->remaining_matches = 0;
        this->initPool(params);
    }

    ~MultiMatcher();

    void insert(const int &id,
                const PCLPointCloud &src,
                const PCLPointCloud &target);
    bool done();
    bool getResult();

 private:
    const int n_thread;
    const int queue_size;
    int remaining_matches;
    R config;
    std::queue<std::tuple<int, PCLPointCloud, PCLPointCloud>> input;
    std::queue<std::tuple<int, Eigen::Affine3d, Mat6>> output;
    std::vector<std::thread> pool;
    std::vector<T> matchers;

    // Synchronization
    std::mutex ip_mutex, op_mutex, cnt_mutex;
    std::condition_variable ip_condition;
    std::condition_variable op_condition;
    bool stop;

    /** Function run by each worker thread
     * @param threadid pair of clouds to match
     */
    void spin(int threadid);
    void initPool(R params);
};

}  // namespace wave

#endif  // WAVE_MULTI_MATCHER_HPP

#include "wave/matching/impl/multi_matcher_impl.hpp"
