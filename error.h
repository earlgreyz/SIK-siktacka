#ifndef ERROR_H
#define ERROR_H

const int RETURN_OK = 0;
const int RETURN_ERROR = 1;


/**
 * Base class for Exception with message.
 */
class Exception: public std::exception {
protected:
    /// Error message
    const std::string message;
public:
    /**
     * Constructs Exception with given message.
     * @param message error message.
     */
    explicit Exception(const std::string &message)
            : message(message) {}

    /**
     * Constructs Exception with given message.
     * @param message error message.
     */
    explicit Exception(std::string &&message)
            : message(std::move(message)) {}

    virtual const char * what() const noexcept override {
        return this->message.c_str();
    }
};

#endif
