#include "../include/Action.h"
#include "../include/Session.h"
#include "../include/User.h"
#include "../include/Watchable.h"

/*constructor*/
BaseAction::BaseAction(): errorMsg(""),status(PENDING) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

/*abstract functions*/
void BaseAction::act(Session &sess) {}

/*change status to COMPLETED*/
void BaseAction::complete() {
    status=COMPLETED;
}

/*change status to ERROR and update the error message */
void BaseAction::error(const std::string &errorMsg) {
    status=ERROR;
    this->errorMsg=errorMsg;
}

/*return the error message*/
std::string BaseAction::getErrorMsg() const {
    return errorMsg;
}

/*return the status in string format*/
std::string BaseAction::status_toString() const {
    if (getStatus()==0)
        return "PENDING";
    else if (getStatus()==1)
        return "COMPLETE";
    else // if getStatus()==2
        return "ERROR";
}

/*print the error message*/
void BaseAction::print_errorMsg() const {
    std::cout<<errorMsg;
}

/*check if the username is in the userMap*/
bool BaseAction::checkname(Session &sess, std::string name) {
    for (auto &it:sess.get_userMap()) {
        if (name.compare(it.first) == 0) {// check if the name exists
            return true;
        }
    }
    return false;
}

/*create a new user*/
void CreateUser::act(Session &sess) {
    sess.AddAct(*this);
    std::string name=sess.GetWord(sess.GetInput(),1);// first word is the name
    std::string algo=sess.GetWord(sess.GetInput(),2);// second word is the algorithm
    if (checkname(sess,name)) {// check if there's another user with the same name
        error("username is already in use");
        std::cout <<"Error - "<<this->getErrorMsg()<<"\n";
    }
    if (getStatus()!=ERROR) {
        if (algo.compare("len") == 0) {// if the algorithm is len
            User *user = new LengthRecommenderUser(name);// create a new user
            sess.edit_userMap({name, user});// add to map
            complete();// update status
        }
        else if (algo.compare("rer") == 0) {// if the algorithm is rer
            User *user = new RerunRecommenderUser(name);// create a new user
            sess.edit_userMap({name, user});// add to map
            complete();// update status
        }
        else if (algo.compare("gen") == 0) {// if the algorithm is gen
            User *user = new GenreRecommenderUser(name);// create a new user
            sess.edit_userMap({name, user});// add to map
            complete();// update status
        }
        else {
            error("there is no such algorithm");
            std::cout <<"Error - "<<this->getErrorMsg()<<"\n";
        }
    }
}
std::string CreateUser::toString() const {
    return "CreateUser "+status_toString();
}
/*return copy of command*/
BaseAction* CreateUser::clone() {
    return new CreateUser(*this);
}

/*change the active user*/
void ChangeActiveUser::act(Session &sess) {
    sess.AddAct(*this);
    if (checkname(sess,sess.GetWord(sess.GetInput(),1))) { // check if the name exists in the userMap
        sess.SetActiveUser(sess.GetWord(sess.GetInput(),1)); // change the active user
        complete();//update status
    }
    else {// if the name does not exist in the userMap
        error("username does not exist");
        std::cout <<"Error - "<<this->getErrorMsg()<<"\n";
    }
}
std::string ChangeActiveUser::toString() const {
    return "ChangeActiveUser "+status_toString();
}
/*return copy of command*/
BaseAction* ChangeActiveUser::clone() {
    return new ChangeActiveUser(*this);
}

/*delete a user*/
void DeleteUser::act(Session &sess) {
    sess.AddAct(*this);
    if (checkname(sess,sess.GetWord(sess.GetInput(),1))) {// check if the name exists in the userMap
        sess.DelUser(sess.GetWord(sess.GetInput(),1));// delete user from userMap
        complete();//update status
    }
    else {// if the name does not exist in the userMap
        error("username does not exist");
        std::cout << "Error - " << this->getErrorMsg();
    }
}
std::string DeleteUser::toString() const {
    return "DeleteUser "+status_toString();
}
/*return copy of command*/
BaseAction* DeleteUser::clone() {
    return new DeleteUser(*this);
}

/*duplicate a user and change the name*/
void DuplicateUser::act(Session &sess) {
    sess.AddAct(*this);
    std::string original_name = sess.GetWord(sess.GetInput(),1);// first word is the original name
    std::string new_name = sess.GetWord(sess.GetInput(),2);// second word is the new name
    if (checkname(sess, new_name)) {// check if there's another user with the same name
        error("username is already in use");
        std::cout <<"Error - "<<this->getErrorMsg()<<"\n";
    }
    else{
        for (auto &it:sess.get_userMap()) {//find the original user
            if (original_name.compare(it.first) == 0) {// check if the name exists
                User *new_user= nullptr;
                //create a new user with the new name and the  same algorithm as the original one
                if (it.second->get_algo().compare("len")==0) {
                    new_user = new LengthRecommenderUser(new_name);
                    for (Watchable* &w: it.second->get_history()){
                        ((LengthRecommenderUser*)new_user)->update_average_length(w->get_length());
                    }
                }
                else if (it.second->get_algo().compare("rer")==0)
                    new_user= new RerunRecommenderUser(new_name);
                else if (it.second->get_algo().compare("gen")==0) {
                    new_user = new GenreRecommenderUser(new_name);
                    for (Watchable* &w: it.second->get_history()){
                        ((GenreRecommenderUser*)new_user)->update_tagNames(w);
                    }
                }
                for (auto &t: sess.get_userMap()){
                    if (t.first.compare(original_name)==0){
                        for (Watchable* w: t.second->get_history()){
                            new_user->edit_history(w);
                        }
                    }
                }
                new_user->copy_history(sess);// copy the watch history of the original user
                std::pair<std::string,User*> p={new_user->getName(),new_user};
                sess.edit_userMap(p);
                complete();// update status
                break;
            }
        }
        if (getStatus()!=COMPLETED) {// if there is no user with the original user name
            error("username does not exist");
            std::cout << "Error - " << this->getErrorMsg();
        }
    }
}
std::string DuplicateUser::toString() const {
    return "DuplicateUser "+status_toString();
}
/*return copy of command*/
BaseAction* DuplicateUser::clone() {
    return new DuplicateUser(*this);
}

/*print the content*/
void PrintContentList::act(Session &sess) {
    sess.AddAct(*this);
    sess.PrintContent();
    complete();
}
std::string PrintContentList::toString() const {
    return "PrintContentList "+status_toString();
}
/*return copy of command*/
BaseAction* PrintContentList::clone() {
    return new PrintContentList(*this);
}

/*print history*/
void PrintWatchHistory::act(Session &sess) {
    sess.AddAct(*this);
    std::cout<<"Watch history for "<<sess.GetActiveUser()->getName()<<"\n";
    for (size_t i=0;i<sess.GetActiveUser()->get_history().size();i++) {
        std::cout <<i+1<< ". "<<(sess.GetActiveUser()->get_history())[i]->toString()<<"\n";
    }
    complete();
}
std::string PrintWatchHistory::toString() const {
    return "PrintWatchHistory "+status_toString();
}
/*return copy of command*/
BaseAction* PrintWatchHistory::clone() {
    return new PrintWatchHistory(*this);
}

/* watch the content and recommend a new one*/
void Watch::act(Session &sess) {
    sess.AddAct(*this);// update action log
    size_t id=std::stol(sess.GetWord(sess.GetInput(),1));// get id of the content
    if (id<1||id>sess.get_content().size()){
        error("wrong id\n");
        std::cout << "Error - " << this->getErrorMsg();
    }
    else {
        std::cout << "Watching " << sess.get_content()[id - 1]->toString() << "\n";
        sess.GetActiveUser()->edit_history(sess.get_content()[id - 1]);// add content to history
        complete();//update status
        Watchable *w = sess.get_content()[id - 1]->getNextWatchable(sess);// get recommended content
        if (w != nullptr) {
            std::cout << "We recommend watching " << w->toString()
                      << ", continue watching? [y/n]\n";
            std::string answer;
            getline(std::cin, answer);
            if (answer.compare("y") == 0) {// if user accepts the recommendation
                long id = w->get_id();// update content id to be the recommended one
                sess.set_input(std::to_string(id));
                Watch *w = new Watch();
                w->act(sess);// watch
            }
        }
    }
}
std::string Watch::toString() const {
    return "Watch "+status_toString();
}
/*return copy of command*/
BaseAction* Watch::clone() {
    return new Watch(*this);
}

/* print the action log*/
void PrintActionsLog::act(Session &sess) {
    sess.AddAct(*this);
    sess.PrintLog();
    complete();
}
std::string PrintActionsLog::toString() const {
    return "PrintActionsLog "+status_toString();
}
/*return copy of command*/
BaseAction* PrintActionsLog::clone() {
    return new PrintActionsLog(*this);
}

/*exit main loop*/
void Exit::act(Session &sess) { // need to do more?
    sess.AddAct(*this);
    complete();
}
std::string Exit::toString() const {
    return "Exit "+status_toString();
}
/*return copy of command*/
BaseAction* Exit::clone() {
    return new Exit(*this);
}




