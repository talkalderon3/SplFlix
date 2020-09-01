#include "../include/Watchable.h"
#include "../include/Session.h"
#include "../include/User.h"
#include <iostream>

/*constructor*/
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags): id(id), length(length), tags(tags) {}

/*destructor*/
Watchable::~Watchable()=default;


/*return the length of the movie/episode*/
int Watchable::get_length() const {
    return length;
}

/*return the id of the movie/episode*/
long Watchable::get_id() const {
    return id;
}

/*return the tag vector of the movie/episode*/
std::vector<std::string> Watchable::get_tags() const {
    return tags;
}

/*check if the tag is in the tag vector*/
bool Watchable::inTags(std::string tag) const {
    for(auto &t:this->tags) {
        if (t.compare(tag) == 0)
            return true;
    }
    return false;
}

/* Movie constructor*/
Movie::Movie(long id, const std::string &name, int length, const std::vector<std::string> &tags):Watchable(id, length,tags) , name(name) {}

/* Movie toString: returns the name of the movie*/
std::string Movie::toString() const {
    return name;
}

/*return the next recommended content to watch according to the user's algorithm*/
Watchable* Movie::getNextWatchable(Session &sess) const {
    return sess.GetActiveUser()->getRecommendation(sess);
}

Watchable* Movie::clone() {
    return new Movie(*this);
}

/* Episode constructor*/
Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags):Watchable(id, length,tags) , seriesName(seriesName), season(season), episode(episode),nextEpisodeId(0){}

/* Episode toString : return "<episode name> S<season number>E<episode number>"*/
std::string Episode::toString() const {
    return seriesName+" "+"S"+std::to_string(season)+"E"+std::to_string(episode);
}

/*returns the next episode to in the tv serie
 * if the is no episode following the current one - recommend content to watch according to the user's algorithm*/
Watchable* Episode::getNextWatchable(Session &sess) const {
    Episode *e=(Episode*)sess.GetActiveUser()->get_history()[sess.GetActiveUser()->get_history().size()-1];
    if (e->get_nextEpisodeId()!=0){
        if (sess.GetActiveUser()->get_algo().compare("len")==0)
            ((LengthRecommenderUser*)(sess.GetActiveUser()))->update_average_length(sess.get_content()[e->get_id()-1]->get_length());
        else if (sess.GetActiveUser()->get_algo().compare("gen")==0)
            ((GenreRecommenderUser*)(sess.GetActiveUser()))->update_tagNames(sess.get_content()[e->get_id()-1]);
        return sess.get_content()[e->get_id()];
    }
    return sess.GetActiveUser()->getRecommendation(sess);
}

/*return the following episode's id*/
long Episode::get_nextEpisodeId() {
    return nextEpisodeId;
}

/*set the following episode's id*/
void Episode::set_nextEpisodeId(long id) {
    nextEpisodeId=id;
}

Watchable* Episode::clone() {
    return new Episode(*this);
}
