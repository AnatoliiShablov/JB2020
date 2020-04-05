#include <algorithm>
#include <iostream>
#include <type_traits>
#include <vector>

struct point;

bool less_by_xy(point const &lhs, point const &rhs) noexcept;
bool less_by_y(point const &lhs, point const &rhs) noexcept;
bool operator==(point const &lhs, point const &rhs) noexcept;

struct point {
  int32_t x;
  int32_t y;

  friend bool less_by_xy(point const &lhs, point const &rhs) noexcept {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
  }

  friend bool less_by_y(point const &lhs, point const &rhs) noexcept {
    return lhs.y < rhs.y;
  }
  friend bool operator==(point const &lhs, point const &rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
};

class min_distance_finder {
 public:
  min_distance_finder(std::vector<point> &&points)
      : points{std::move(points)},
        points_merge_tmp{},
        result{std::numeric_limits<uint64_t>::max()} {}

  [[nodiscard]] uint64_t value() noexcept {
    if (result == std::numeric_limits<uint64_t>::max()) {
      std::sort(points.begin(), points.end(), less_by_xy);
      for (size_t i = 1; i < points.size(); ++i) {
        if (points[i] == points[i - 1]) {
          result = 0;
        }
      }
      if (result != 0) {
        points_merge_tmp.resize(points.size());
        value_impl(points.begin(), points.end());
        points.clear();
        points.shrink_to_fit();
      }
    }
    return result;
  }

 private:
  [[nodiscard]] uint64_t sqr(int64_t value) const noexcept {
    return static_cast<uint64_t>(value * value);
  }

  [[nodiscard]] uint64_t abs(int32_t value) const noexcept {
    return static_cast<uint64_t>(value < 0 ? -value : value);
  }

  [[nodiscard]] uint64_t distance(point const &lhs, point const &rhs) const
      noexcept {
    return sqr(static_cast<int64_t>(lhs.x) - static_cast<int64_t>(rhs.x)) +
           sqr(static_cast<int64_t>(lhs.y) - static_cast<int64_t>(rhs.y));
  }

  void update(point const &lhs, point const &rhs) noexcept {
    if (uint64_t dist = distance(lhs, rhs); dist < result) {
      result = dist;
    }
  }

  void merge(std::vector<point>::iterator begin,
             std::vector<point>::iterator middle,
             std::vector<point>::iterator end) noexcept {
    auto begin_1 = begin;
    auto end_1 = middle;

    auto begin_2 = middle;
    auto end_2 = end;

    auto output_begin = points_merge_tmp.begin();

    while (begin_1 < end_1 && begin_2 < end_2) {
      *(output_begin++) =
          less_by_y(*begin_1, *begin_2) ? *(begin_1++) : *(begin_2++);
    }

    while (begin_1 < end_1) {
      *(output_begin++) = *(begin_1++);
    }

    while (begin_2 < end_2) {
      *(output_begin++) = *(begin_2++);
    }

    std::copy(points_merge_tmp.begin(), output_begin, begin);
  }

  void value_impl(std::vector<point>::iterator begin,
                  std::vector<point>::iterator end) noexcept {
    if (end - begin <= MAX_BROOT) {
      for (auto i = begin; i < end; ++i) {
        for (auto j = i + 1; j < end; ++j) {
          update(*i, *j);
        }
      }
      std::sort(begin, end, less_by_y);
      return;
    }

    auto middle = begin + (end - begin) / 2;

    value_impl(begin, middle);
    value_impl(middle, end);

    merge(begin, middle, end);

    int32_t middle_x_value = middle->x;
    auto good_zone = points_merge_tmp.begin();
    for (auto it = begin; it < end; ++it)
      if (abs(middle_x_value - it->x) < result) {
        for (auto gz_it = points_merge_tmp.begin();
             gz_it < good_zone && abs(it->y - gz_it->y) < result; ++gz_it) {
          update(*it, *gz_it);
        }
        *(good_zone++) = *it;
      }
  }

  static constexpr ptrdiff_t MAX_BROOT = 5;
  std::vector<point> points;
  std::vector<point> points_merge_tmp;
  uint64_t result;
};

int main() {
  size_t amount_of_points;
  std::cin >> amount_of_points;
  std::vector<point> points(amount_of_points);

  for (size_t i = 0; i < amount_of_points; ++i) {
    std::cin >> points[i].x >> points[i].y;
  }
  std::cout << min_distance_finder(std::move(points)).value();
  return 0;
}
