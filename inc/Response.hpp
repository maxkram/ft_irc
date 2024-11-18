#pragma once
class Response {
    public:
        Response();
        ~Response();
    private:
        std::string _version;
        std::string _code;
        std::string _status;
};