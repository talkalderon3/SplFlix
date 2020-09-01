#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>


class Watchable;
class Session;

class User{
public:
    User(const std::string& name);/*constructor*/
    virtual ~User();
    virtual Watchable* getRecommendation(Session& s) = 0;/*abstract function*/
    std::string getName() const;/*return the user's name*/
    std::vector <Watchable*> get_history();/*return a copy of history*/
    /*new*/
    void edit_history (Watchable* content);/*add content to history*/
    void set_history(std::vector <Watchable*> history);/*set history*/
    std::string get_algo() const;/*return the algorithm's type*/
    bool inHistory (long id);/* check if the content is in history*/
    virtual User* clone() = 0;/*abstract function*/
    void copy_history(Session& s);
protected:
    std::vector<Watchable*> history;
    /*new*/
    void set_algo(std::string algo);/*change algo*/
private:
    const std::string name;
    /*new*/
    std::string algo;

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);/*LengthRecommenderUser constructor*/
    virtual ~LengthRecommenderUser(){};
    virtual Watchable* getRecommendation(Session& s);/*LengthRecommenderUser algorithm:
 * recommend new content according to average length*/
    /*new*/
    void update_average_length (long length);
    virtual User* clone();//create a copy of the user
private:
    /*new*/
    long length;
    int count=0;
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);/*RerunRecommenderUser constructor*/
    virtual ~RerunRecommenderUser(){};
    virtual Watchable* getRecommendation(Session& s);/*RerunRecommenderUser algorithm:
 * recommend content in history by order*/
    /*new*/
    virtual User* clone();//create a copy of the user
private:
    /*new*/
    size_t index;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);/*GenreRecommenderUser constructor*/
    virtual ~GenreRecommenderUser(){};
    virtual Watchable* getRecommendation(Session& s);/*GenreRecommenderUser algorithm:
 * recommend new content according to most popular tag in history*/
    /*new*/
    virtual User* clone();//create a copy of the user
    void update_tagNames (Watchable* w);
private:
    /*new*/
    std::vector<std::pair<int,std::string>> tagNames;
    static bool sort_by_tag_name (const std::pair<int,std::string> &a,const std::pair<int,std::string> &b);/*Lexicographical order sort*/
};

#endif