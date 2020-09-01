#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>

class Session;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
    BaseAction();/*constructor*/
    virtual ~BaseAction()= default;
    ActionStatus getStatus() const;
    virtual void act(Session& sess)=0;/*abstract function*/
    virtual std::string toString() const=0;/*abstract function*/
    /*new*/
    std::string status_toString() const;/*return the status in string format*/
    void print_errorMsg () const;/*print the error message*/
    bool checkname (Session &sess,std::string name);/*check if the username is in the userMap*/
    virtual BaseAction* clone()=0;
protected:
    void complete();/*change status to COMPLETED*/
    void error(const std::string& errorMsg);/*change status to ERROR and update the error message */
    std::string getErrorMsg() const;/*return the error message*/
private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    virtual void act(Session& sess);/*create a new user*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class ChangeActiveUser : public BaseAction {
public:
    virtual void act(Session& sess);/*change the active user*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class DeleteUser : public BaseAction {
public:
    virtual void act(Session & sess);/*delete a user*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class DuplicateUser : public BaseAction {
public:
    virtual void act(Session & sess);/*duplicate a user and change the name*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class PrintContentList : public BaseAction {
public:
    virtual void act (Session& sess);/*print the content*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class PrintWatchHistory : public BaseAction {
public:
    virtual void act (Session& sess);/*print history*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class Watch : public BaseAction {
public:
    virtual void act(Session& sess);/* watch the content and recommend a new one*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class PrintActionsLog : public BaseAction {
public:
    virtual void act(Session& sess);/* print the action log*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};

class Exit : public BaseAction {
public:
    virtual void act(Session& sess);/*exit main loop*/
    virtual std::string toString() const;
    /*new*/
    virtual BaseAction* clone();//return copy of command
};
#endif