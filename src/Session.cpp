#include "../include/Session.h"
#include "../include/json.hpp"
#include "../include/Watchable.h"
#include <string>
#include <fstream>
#include "../include/User.h"
#include <unordered_map>
using namespace std;


using nlohmann::json;

/*constructor*/
Session::Session(const std::string &configFilePath) : content(), actionsLog(), userMap(), activeUser(new LengthRecommenderUser ("default")), input(""){
    /* Initialize a default watching user */
    userMap.insert({activeUser->getName(),activeUser}); // Insert the user to the userMap
    /* Read the json file*/
    std::ifstream strm(configFilePath);
    nlohmann::json j = nlohmann::json::parse(strm);
    long id=1;
    //movies
    for(auto &m : j["movies"]){
        content.push_back(new Movie(id,m["name"],m["length"], m["tags"]));// add movie to content
        id++;
    }
    //tv series
    for(auto &t : j["tv_series"]){
        int season=1;
        for(auto &s : t["seasons"]){
            for (int i=1; i<=s;i++){
                Episode *e=new Episode(id,t["name"],t["episode_length"],season, i,t["tags"]);
                e->set_nextEpisodeId(id+1);
                content.push_back(e);// add episode to content
                id++;
            }
            season++;
        }
        Episode *e=(Episode*)content[content.size()-1];
        e->set_nextEpisodeId(0);// the last episode of the season's next id=0
    }
}
//Copy Constructor
Session::Session(const Session &other):content(),actionsLog(),userMap(), activeUser(),input(other.input) {
    for (Watchable *con:other.content) {
        this->content.push_back(con->clone());
    }
    for (BaseAction* log : other.actionsLog) {
        this->actionsLog.push_back(log->clone());
    }
    for (auto &user_other : other.userMap) {
        std::pair<std::string, User *> p = {user_other.first, user_other.second->clone()};
        p.second->copy_history(*this);
        this->userMap.insert(p); // check
    }
    for (auto &user: userMap) {
        if (user.first.compare(other.activeUser->getName()) == 0) {
            this->activeUser = user.second;
            break;
        }
    }
}
//Copy Assignment
Session& Session::operator=(const Session &other) {
    if (this == &other) {
        return *this;
    }
    clean();
    for (Watchable *con:other.content) {
        this->content.push_back(con->clone());
    }
    for (BaseAction* log : other.actionsLog) {
        this->actionsLog.push_back(log->clone());
    }
    for (auto &user_other : other.userMap) {
        std::pair<std::string, User *> p = {user_other.first, user_other.second->clone()};
        p.second->copy_history(*this);
        this->userMap.insert(p); // check
    }
    for (auto &user: userMap) {
        if (user.first.compare(other.activeUser->getName()) == 0) {
            this->activeUser = user.second;
        }

    }
    return *this;
}

//Move Constructor
Session::Session(Session &&other):content(),actionsLog(),userMap(),activeUser(), input(other.input) {
    for(Watchable *con: other.content){
        this->content.push_back(con);
    }
    for(auto &user_other: other.userMap){
        std::pair <std::string, User*> p={user_other.first,user_other.second};
        this->userMap.insert(p); // check
    }
    for(BaseAction *log: other.actionsLog){
        this->actionsLog.push_back(log);
    }
    activeUser=other.activeUser;
    //nulls
    other.activeUser = nullptr;
    other.content.clear();
    other.userMap.clear();
    other.actionsLog.clear();
}

//Move Assignment
Session& Session::operator=(Session &&other) {
    if (this == &other) {
        return *this;
    }
    clean();
    for(Watchable *con: other.content){
        this->content.push_back(con);
    }
    for(auto &user_other: other.userMap){
        std::pair <std::string, User*> p={user_other.first,user_other.second};
        this->userMap.insert(p); // check
    }
    for(BaseAction *log: other.actionsLog){
        this->actionsLog.push_back(log);
    }
    activeUser=other.activeUser;

    //nulls
    other.activeUser = nullptr;
    other.content.clear();
    other.userMap.clear();
    other.actionsLog.clear();
    return *this;
}
/*destructor*/
Session::~Session() {
    clean();
}


/*start()*/
void Session:: start(){
    cout << "SPLFLIX is now on! \n";
    bool StillInLoop=true;
    //main loop
    while (StillInLoop) {
        getline(cin, input);// get command from the user
        string FirstInput = GetWord(input,1);
        input = GetWord(input,2);
        //Create User action
        if (FirstInput.compare("createuser")==0) {
            BaseAction *creatuser = new CreateUser();
            creatuser->act(*this); // using Action act of Create User
        }

        //Change User action
        else if (FirstInput.compare("changeuser")==0) {
            ChangeActiveUser *change = new ChangeActiveUser();
            change ->act(*this);
        }

        // Delete User action
        else if (FirstInput.compare("deleteuser")==0) {
            DeleteUser *del = new DeleteUser();
            del -> act(*this);
        }

        // Duplicate Use action
        else if (FirstInput.compare("dupuser")==0) {
            DuplicateUser *dup = new DuplicateUser();
            dup ->act(*this);
        }

        // Print Content List action
        else if (FirstInput.compare("content")==0) {
            PrintContentList *content = new PrintContentList();
            content ->act(*this);
        }

        // Print Watch History action
        else if (FirstInput.compare("watchhist")==0){
            PrintWatchHistory *hist = new PrintWatchHistory();
            hist ->act(*this);
        }

        // Watch action
        else if (FirstInput.compare("watch")==0) {
            Watch *watch = new Watch();
            watch ->act(*this);
        }

        // Print Actions Log action
        else if (FirstInput.compare("log")==0){
            PrintActionsLog *act_log = new PrintActionsLog();
            act_log ->act(*this);
        }

        //exit
        else if (FirstInput.compare("exit")==0){
            StillInLoop=false;
            Exit *act_log = new Exit();
            act_log ->act(*this);
        }

        // unexpected input
        else {
            cout << "error: invalid command \n";
        }
    }
}

/*return input*/
std::string Session:: GetInput () const {
    return input;
}

/*change input*/
void Session::set_input(std::string input) {
    this->input=input;
}

/*return a copy of the userMap*/
std::unordered_map <std::string,User*> Session::get_userMap() const {
    return userMap;
}

/*add a new pair to the userMap*/
void Session::edit_userMap(std::pair<std::string , User * > p) {
    userMap.insert(p);
}

/*return a copy of the content vector*/
std::vector<Watchable*> Session::get_content() const {
    return content;
}

/*return active user*/
User* Session ::GetActiveUser() const{
    return activeUser;
}

/*add the action to the action log*/
void Session ::AddAct(BaseAction &act) {
    actionsLog.push_back(&act);
}

/*print action log*/
void Session ::PrintLog() {
    for(size_t i = 0; i<=actionsLog.size()-1; i++) {
        cout<<actionsLog[i]->toString();
        if (actionsLog[i]->getStatus()==ERROR){
            std::cout <<":";
            actionsLog[i]->print_errorMsg();
        }
        cout<<"\n";
    }
}

/*print the content*/
void Session ::PrintContent() {
    for (std::size_t i = 0; i < content.size(); i++) {
        cout << i+1 <<". "<<content[i]->toString()<<" "<<content[i]->get_length()<<" minutes [";
        for (std::size_t t=0;t<content[i]->get_tags().size();t++){
            if (t==0)
                cout << content[i]->get_tags()[t];
            else
                cout<< ", " << content[i]->get_tags()[t];
        }
        cout << "]\n";
    }
}

/* Change the active user*/
void Session ::SetActiveUser(string NameNewActiveUser) {
    for (auto &it:userMap){
        if (it.first.compare(NameNewActiveUser)==0) {
            activeUser = it.second;
            break;
        }
    }
}

/* Delete user from userMap*/
void Session::DelUser(std::string UserName) {
    if (inMap(UserName)) {
        delete (userMap.at(UserName));
        userMap.erase(UserName);// delete user name from the userMap.
    }
}




/* Return the i'th word in a string*/
string Session ::GetWord(std::string name, int i) {
    std::size_t space = name.find(" ");
    if (i==1) {
        if (space != string::npos)
            return name.substr(0, space);
        return name;
    }
    else {
        if (space != string::npos)
            return name.substr(space + 1, name.size());
        return "";
    }
}

/*check is user name is in userMap*/
bool Session::inMap(std::string userName) {
    for (auto &it: userMap){
        if (it.first.compare(userName)==0)
            return true;
    }
    return false;
}


/*cleaning function*/
void Session::clean() {
    for (Watchable *c: content) {
        delete c;
    }
    this->content.clear();
    for(auto &users : userMap){
        delete (users.second); // check
    }
    userMap.clear();
    for (size_t i=0;i<actionsLog.size();i++) {
        delete actionsLog[i];
    }
    this->actionsLog.clear();

}