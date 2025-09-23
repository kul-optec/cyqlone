#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <random>
#include <ranges>
#include <vector>

#include <cyqlone/qpalm/implementation/algorithms.hpp>

using cyqlone::qpalm::partition_min;

// Reference implementation: partition, then min_element
template <typename R, typename Pred, typename Cmp>
auto partition_min_ref(R &&range, Pred pred, Cmp cmp) {
    auto part_false = std::ranges::partition(range, pred);
    if (std::ranges::empty(part_false))
        return part_false;
    // Reverse view: ensures last minimum in forward order becomes the chosen one
    auto rev_view   = std::ranges::reverse_view(part_false);
    auto min_it_rev = std::ranges::min_element(rev_view, cmp);
    auto min_it     = std::prev(min_it_rev.base());
    auto boundary   = std::ranges::begin(part_false);
    if (min_it != boundary)
        std::ranges::iter_swap(boundary, min_it);
    return part_false;
}

template <typename Pred, typename Cmp>
void run_test(const std::vector<int> &input, Pred pred, Cmp cmp) {
    auto v1 = input;
    auto v2 = input;

    auto sub1 = partition_min(v1, pred, cmp);
    auto sub2 = partition_min_ref(v2, pred, cmp);

    // Partition property checks
    EXPECT_TRUE(std::ranges::all_of(v1.begin(), sub1.begin(), pred))
        << "Left partition contains an element that fails the predicate";
    EXPECT_TRUE(std::ranges::none_of(sub1.begin(), sub1.end(), pred))
        << "Right partition contains an element that satisfies the predicate";

    // Boundary must be the same
    EXPECT_EQ(std::ranges::distance(v1.begin(), sub1.begin()),
              std::ranges::distance(v2.begin(), sub2.begin()));

    // Final sequences must match
    EXPECT_EQ(v1, v2);
}

struct PartitionMinTest : public ::testing::Test {
    std::less<> cmp{};
    std::function<bool(int)> pred = [](int x) { return x % 2 == 0; }; // even
};

TEST_F(PartitionMinTest, emptyRange) { run_test({}, pred, cmp); }
TEST_F(PartitionMinTest, allTruePartition) { run_test({2, 4, 6}, pred, cmp); }
TEST_F(PartitionMinTest, allFalsePartition) { run_test({1, 3, 5}, pred, cmp); }
TEST_F(PartitionMinTest, singleElementTrue) { run_test({42}, pred, cmp); }
TEST_F(PartitionMinTest, singleElementFalse) { run_test({43}, pred, cmp); }
TEST_F(PartitionMinTest, mixedPartitions) { run_test({5, 2, 7, 4, 6, 3}, pred, cmp); }
TEST_F(PartitionMinTest, alreadyPartitioned) { run_test({2, 4, 6, 5, 3, 1}, pred, cmp); }
TEST_F(PartitionMinTest, reversePartitioned) { run_test({5, 3, 1, 2, 4, 6}, pred, cmp); }
TEST_F(PartitionMinTest, decreasingSequence) { run_test({10, 9, 8, 7, 6, 5}, pred, cmp); }
TEST_F(PartitionMinTest, duplicateMinima) { run_test({7, 3, 5, 3, 9, 2}, pred, cmp); }
TEST_F(PartitionMinTest, allDuplicateMinima) { run_test({3, 3, 4, 3, 3, 2}, pred, cmp); }

TEST_F(PartitionMinTest, randomizedStress) {
    std::mt19937 rng{12345};
    std::uniform_int_distribution<int> dist{0, 99};

    std::vector<int> v;
    for (int t = 0; t < 10000; ++t) {
        int n = dist(rng) % 20;
        v.resize(n);
        for (int i = 0; i < n; ++i)
            v[i] = dist(rng);
        run_test(v, pred, cmp);
    }
}
