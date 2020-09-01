#ifndef WATCHABLE_H_
#define WATCHABLE_H_

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);/*constructor*/
    virtual ~Watchable();/*destructor*/
    virtual std::string toString() const = 0;/*abstract functions*/
    virtual Watchable* getNextWatchable(Session&) const = 0;/*abstract functions*/
    /*new*/
    int get_length() const;/*return the length of the movie/episode*/
    long get_id() const;/*return the id of the movie/episode*/
    std::vector<std::string> get_tags() const;/*return the tag vector of the movie/episode*/
    bool inTags(std::string tag) const;/*check if the tag is in the tag vector*/
    virtual Watchable* clone() = 0;/*abstract function*/
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);/* Movie constructor*/
    virtual ~Movie(){};
    virtual std::string toString() const;/* Movie toString: returns the name of the movie*/
    virtual Watchable* getNextWatchable(Session&) const;/*return the next recommended content to watch according to the user's algorithm*/
    /*new*/
    virtual Watchable* clone();//return copy of movie
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);/* Episode constructor*/
    virtual ~Episode(){};
    virtual std::string toString() const;/* Episode toString : return "<episode name> S<season number>E<episode number>"*/
    virtual Watchable* getNextWatchable(Session&) const;/*returns the next episode in the tv serie
 * if the is no episode following the current one - recommend content to watch according to the user's algorithm*/
    /*new*/
    long get_nextEpisodeId();/*return the following episode's id*/
    void set_nextEpisodeId(long id);/*set the following episode's id*/
    virtual Watchable* clone();//return copy of episode
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif