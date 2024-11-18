#pragma once
enum Method {
    GET,
    POST,
    DELETE,
    NONE
};
class Request {
    public:
        Request();
        ~Request();
    private:
        Method _method;
};