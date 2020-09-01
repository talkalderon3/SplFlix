#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <algorithm>
#include <cmath>
using namespace std;

/*constructor*/
User::User(const std::string &name):history(),name(name), algo("len"){}

User::~User() {};

/*return the user's name*/
std::string User::getName() const {
    return name;
}

/*return a copy of history*/
std::vector<Watchable *> User::get_history() {
    return history;
}

/*add content to history*/
void User::edit_history(Watchable *content) {
    this->history.push_back(content);
}

/*set history*/
void User::set_history(std::vector<Watchable *> history) {
    this->history=history;
}

/*return the algorithm's type*/
std::string User::get_algo() const {
    return algo;
}

/*change algo*/
void User::set_algo(std::string algo) {
    this->algo=algo;
}

/* check if the content is in history*/
bool User::inHistory(long id) {
    for (auto h = history.begin(); h != history.end(); h++) {
        if (id== h.operator*()->get_id())
            return true;
    }
    return false;
}


void User::copy_history(Session &s) {
    for (std::size_t i=0; i<history.size();i++) {
        history[i]=s.get_content()[history[i]->get_id()-1];
    }
}

/*LengthRecommenderUser constructor*/
LengthRecommenderUser::LengthRecommenderUser(const std::string &name): User(name), length(0), count(0){
    set_algo("len");
}

/*LengthRecommenderUser algorithm: recommend new content according to average length*/
Watchable* LengthRecommenderUser::getRecommendation(Session &s) {
    //update the average length of the content in history
    update_average_length(history[history.size()-1]->get_length());
    long average_length=(length/count);
    // find the content with a length closest to the average that hasn't been seen yet
    double shortest=INFINITY;
    Watchable *next=nullptr;
    for (Watchable *w:s.get_content()){
        if (abs(w->get_length()-average_length)<shortest){
            if (!(this->inHistory(w->get_id()))) {
                next = w;
                shortest = abs(w->get_length() - average_length);
            }
        }
    }
    return next;
}

void LengthRecommenderUser::update_average_length(long length) {
    this->length=this->length+length;
    this->count++;
}

/*create a copy of the user*/
User* LengthRecommenderUser::clone() {
    return new LengthRecommenderUser(*this);
}

/*RerunRecommenderUser constructor*/
RerunRecommenderUser::RerunRecommenderUser(const std::string &name) : User(name), index(0){
    set_algo("rer");
}

/*RerunRecommenderUser algorithm: recommend content in history by order*/
Watchable* RerunRecommenderUser::getRecommendation(Session &s) {
    Watchable* w=history[index];
    if (index+1<history.size()){
        index++;
    }
    else{
        index=0;
    }
    return w;
}

/*create a copy of the user*/
User* RerunRecommenderUser::clone() {
    return new RerunRecommenderUser(*this);
}
/*GenreRecommenderUser constructor*/
GenreRecommenderUser::GenreRecommenderUser(const std::string &name) : User(name), tagNames() {
    set_algo("gen");
}

/*GenreRecommenderUser algorithm: recommend new content according to most popular tag in history*/
Watchable* GenreRecommenderUser::getRecommendation(Session &s) {
    update_tagNames(history[history.size()-1]);
    sort(tagNames.begin(), tagNames.end(),sort_by_tag_name);// sort first by counter then by name
    for (auto &t: tagNames){
        for (auto &c:s.get_content()){
            if (c->inTags(t.second)&!inHistory(c->get_id())){// if the content isn't in historyg
                return c;//return recommended content
            }
        }
    }
    return nullptr;
}

/*Lexicographical order sort*/
bool GenreRecommenderUser::sort_by_tag_name(const std::pair<int, std::string> &a, const std::pair<int, std::string> &b) {
    return (a.second.compare(b.second)>0);
}

/*create a copy of the user*/
User* GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);
}

void GenreRecommenderUser::update_tagNames(Watchable* w) {
    for (auto &t: w->get_tags()){
        if (tagNames.size()==0)
            tagNames.push_back(make_pair(1, t));// if tagName is empty - add the tag with counter=1
        else {
            bool added=false;
            for (auto &n: tagNames) {
                if ((n.second).compare(t) == 0) {// if the tag is in tagNames
                    n.first=n.first+1;//update it's counter
                    added=true;
                }
            }
            if (!added)// if the tag is'nt in tagNames
                tagNames.push_back(make_pair(1, t));//add the tag with counter=1
        }
    }
}


