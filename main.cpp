#include <iostream>
#include "BallSort.h"
#include<cstring>

int main() {
#ifdef __EMSCRIPTEN__
return 0;
#endif
    BallSort<14,4> balls;
    std::cout << balls.render() << '\n';
    std::cout << balls.is_solved() << '\n';
    auto path = BallSort<14,4>::solve(balls);
    for(auto & s : path){
        std::cout << s.render() << '\n';
    }
    std::cout << path.size() << " steps\n";
//    balls.try_swap(1, 8);
//    std::cout << balls.render() << '\n';

    return 0;
}

char* make_cstr(const std::string & s){
    char * cstr = new char[s.size() + 1];
    strcpy(cstr, s.c_str());
    return cstr;
}

#ifdef __EMSCRIPTEN__
#include<emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE char* hi(){
        BallSort<14, 4> balls;
        return make_cstr(balls.render());
    }

    EMSCRIPTEN_KEEPALIVE void free_str(char* s){
        delete[] s;
    }

    EMSCRIPTEN_KEEPALIVE char* solution(){
        BallSort<14, 4> balls;
        auto sol = BallSort<14,4>::solve(balls);

        std::string ret;
        for(auto & b : sol){
            ret += b.render();
            ret += '|';
        }
        ret.pop_back();
        return make_cstr(ret);
    }
}


#endif

