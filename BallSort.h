//
// Created by 16182 on 11/17/2022.
//

#ifndef BALLSORT_BALLSORT_H
#define BALLSORT_BALLSORT_H

#include<array>
#include<cstdint>
#include<vector>
#include<string>
#include<cassert>
#include<algorithm>
#include<random>
#include<queue>
#include<unordered_set>
#include<string_view>
#include<unordered_map>
#include<iostream>

struct Ball {
    int8_t val = -1;
    Ball() = default;
    Ball(const Ball&) = default;
    Ball& operator=(const Ball&) = default;
    bool operator==(const Ball&) const = default;
    Ball(int i): val(i){
        assert(i<128);
    }
    bool is_empty() const {
        return val == -1;
    }

    static Ball empty() {
        return {-1};
    }

    char to_char() const {
        return val == -1 ? '.' : 'a' + val;
    }
};

template<int N>
struct Column {
    Column() = default;
    template<typename iter>
    Column(iter a, iter b){
        for(int i = 0; a != b; a++, i++){
            assert(i < N);
            cells[i] = *a;
        }
    }

    Column(const Column<N>&other) = default;
    Column<N>& operator=(const Column<N>&) = default;
    bool operator<=>(const Column<N>&) const = default;

    void swap_to(Column<N> & dest){
        Ball top = top_ball();
        dest.drop(top);
        cells[top_idx()] = Ball::empty();
    }

    bool can_swap(const Column<N> & dest) const {
        Ball top = top_ball();
        if(top.is_empty()) return false;
        return dest.can_drop(top);
    }

    void drop(Ball b) {
        assert(top_idx() != N - 1);
        cells[top_idx() + 1] = b;
    }

    bool can_drop(Ball b) const {
        if(cells[N-1].is_empty()){
            return top_ball() == b || top_ball() == Ball::empty();
        } else {
            return false;
        }
    }

    int top_idx() const {
        for(int i = N-1; i >= 0; i--){
            if(!cells[i].is_empty()){
                return i;
            }
        }
        return -1;
    }

    Ball top_ball() const {
        for(int i = N-1; i >= 0; i--){
            if(!cells[i].is_empty()){
                return cells[i];
            }
        }
        return {};
    }

    bool is_solved() const {
        for(int i = 0; i < N; i++){
            if(cells[i] != cells[0]) return false;
        }
        return true;
    }

    uint16_t bit_repr() const {
        static_assert(N == 4);

        return
          (uint16_t(cells[0].val & 0xf) << 0)
        + (uint16_t(cells[1].val & 0xf) << 4)
        + (uint16_t(cells[2].val & 0xf) << 8)
        + (uint16_t(cells[3].val & 0xf) << 12);
    }

    //0 = bottom, N-1 = top
    std::array<Ball, N> cells{};
};

template<int W, int N>
class BallSort {
    static_assert(W > 2);
public:
    bool operator==(const BallSort& b) const = default;
    bool operator!=(const BallSort& b) const = default;

    using BitRepr = std::array<uint16_t, W>;
    BitRepr bit_repr() const {
        std::array<uint16_t, W> ret{};
        for(int i = 0; i < W; i++){
            ret[i] = columns[i].bit_repr();
        }
        std::sort(ret.begin(), ret.begin() + W);
        return ret;
    }

    std::array<Column<N>, W> columns;
    explicit BallSort() {
        std::vector<Ball> balls;
        for(int i = 0; i < W-2; i++){
            for(int b = 0; b < N; b++){
                balls.push_back({i});
            }
        }
        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(balls.begin(), balls.end(), g);

        for(int i = 0; i < W-2; i++){
            columns[i] = Column<N>(balls.begin() + i*N, balls.begin() + i*N+N);
        }
    }

    bool is_solved() const {
        return std::all_of(columns.begin(), columns.end(), [](const auto& c){return c.is_solved();});
    }

    void swap_cols(int src, int dest){
        return columns[src].swap_to(columns[dest]);
    }

    bool can_swap(int src, int dest) const {
        return columns[src].can_swap(columns[dest]);
    }

    std::string render() const {
        std::string ret;
        for(int i = N-1; i >= 0; i--){
            for(int c = 0; c < columns.size(); c++){
                ret += std::to_string(columns[c].cells[i].val);
                ret += ' ';
            }
            ret.pop_back();
            ret += '\n';
        }
        return ret;
    }

    struct BitReprHash {
        size_t operator()(const BitRepr& r) const {
            size_t h = 0;
            for(auto i : r){
                h = h * 31 + i;
            }
            return h;
        }
    };

    static std::vector<BallSort> solve(BallSort start) {
        std::unordered_map<BitRepr, std::pair<uint8_t,uint8_t>, BitReprHash> seen;
        std::queue<BallSort<W,N>> to_search;
        to_search.push(start);

        while(!to_search.empty()){
            BallSort search = to_search.front();
            to_search.pop();
            for(int i = 0; i < search.columns.size(); i++){
                for(int j = 0; j < search.columns.size(); j++){
                    if(j != i && search.can_swap(i,j)){
                        auto cpy = search;
                        cpy.swap_cols(i, j);
                        BitRepr rep = cpy.bit_repr();
                        if(!seen.contains(rep)){
                            seen.emplace(rep, std::pair<uint8_t, uint8_t>{i,j});
                            to_search.push(cpy);
                            if(cpy.is_solved()){
                                std::cout << seen.size() << " searched boards\n";
                                return reconstruct_sol(seen, start, cpy);
                            }
                        }
                    }
                }
            }
        }

        std::cout << seen.size() << " searched boards\n";
        return {};
    }

private:
    static std::vector<BallSort> reconstruct_sol(std::unordered_map<BitRepr, std::pair<uint8_t, uint8_t>, BitReprHash>& seen, BallSort start, BallSort end){
        std::vector<BallSort> path;
        path.push_back(end);

        while(path.back() != start){
            BitRepr r = path.back().bit_repr();
            auto [s,e] = seen[r];
            end.swap_cols(e,s);
            path.push_back(end);
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};


#endif //BALLSORT_BALLSORT_H
