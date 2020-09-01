#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);//constructor
    Session( const Session &other);//Copy Constructor
    Session&operator=(const Session &other);//Copy Assignment
    Session(Session &&other);//Move Constructor
    Session&operator=(Session &&other);//Move Assignment
    ~Session();//destructor
    void start();// start()
    /*new*/
    std::string GetInput() const ;// return input
    void set_input(std::string input) ;// change input
    std::unordered_map<std::string,User*> get_userMap() const;// return a copy of the userMap
    void edit_userMap(std::pair<std::string,User*>);//add a new pair to the userMap
    std::vector<Watchable*> get_content() const;//return a copy of the content vector
    User* GetActiveUser() const ;// return active user
    void AddAct (BaseAction &act); // add the action to the action log
    void PrintLog();// print action log
    void PrintContent ();// print the content
    void SetActiveUser(const std::string );//Change the active user
    void DelUser (std::string s) ;//Delete user from userMap
    std::string GetWord(std::string, int);//Return the i'th word in a string
    bool inMap (std::string userName);//check is user name is in userMap
    void clean ();
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    /*new*/
    std::string input;
};
#endif