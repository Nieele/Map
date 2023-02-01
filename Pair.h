//
// Created by Никита on 23.11.2022.
//

#ifndef INC_1_PAIR_H
#define INC_1_PAIR_H

namespace lab {
    template<typename T, typename U>
    class Pair {
    public:
        T first;
        U second;
        bool nullEl = true;
    public:
        Pair() = default;
        Pair(const T frst) : first(frst), nullEl(true) {}
        Pair(const T frst, const U scnd) : first(frst), second(scnd), nullEl(false) {}

        template<typename OT, typename OU>
        friend std::ostream& operator<<(std::ostream& out, const Pair<OT, OU>& pr) {
            if (!pr.nullEl) out << pr.first << " " << pr.second;
            else out << pr.first << " " << "Null";
            return out;
        }

        bool isNull() { return nullEl; }

        bool operator<(const Pair<T, U> other) const { return first < other.first; }
        bool operator>(const Pair<T, U> other) const { return first > other.first; }
        bool operator==(const Pair<T, U> other) const { return first == other.first; }
        bool operator!=(const Pair<T, U> other) const { return first != other.first; }
        bool operator<=(const Pair<T, U> other) const { return first <= other.first; }
        bool operator>=(const Pair<T, U> other) const { return first >= other.first; }
    };
}

#endif //INC_1_PAIR_H
