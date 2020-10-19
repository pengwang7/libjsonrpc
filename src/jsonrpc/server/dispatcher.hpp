#ifndef __JSONRPC_DISPATCHER_HPP__
#define __JSONRPC_DISPATCHER_HPP__


#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include <utility>
#include <functional>

#include "callback.hpp"
#include "integer_seq.hpp"

namespace jsonrpc {

class MethodWrapper {
public:
    typedef std::function<std::string(const std::string&)> Method;

    explicit MethodWrapper(Method method) : method_(method) {}

    MethodWrapper(const MethodWrapper&) = delete;

    MethodWrapper& operator=(const MethodWrapper&) = delete;

    std::string operator()(const std::string& params) const { return method_(params); }

private:
    Method method_;
};

class Dispatcher {
public:
    MethodWrapper& addMethod(std::string name, MethodWrapper::Method method) {
        std::unique_lock<std::mutex> lock(methods_mutex_);

        auto result = methods_.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(std::move(name)),
            std::forward_as_tuple(std::move(method))
        );

        lock.unlock();

        if (!result.second) {
            throw std::invalid_argument(name + ": method already added");
        }

        return result.first->second;
    }

    template<typename T>
    MethodWrapper& addMethod(std::string name, std::string(T::*method)(const std::string&), T& instance) {
        return addMethod(std::move(name), std::bind(method, &instance, std::placeholders::_1));
    }

    template<typename T>
    MethodWrapper& addMethod(std::string name, std::string(T::*method)(const std::string&) const, T& instance) {
        return addMethod(std::move(name), std::bind(method, &instance, std::placeholders::_1));
    }

    template<typename ReturnType, typename T, typename... ParameterTypes>
    MethodWrapper& addMethod(std::string name, ReturnType(T::*method)(ParameterTypes...), T& instance) {
        std::function<ReturnType(ParameterTypes...)> function = [&instance, method](ParameterTypes&&... params) -> ReturnType {
            return (instance.*method)(std::forward<ParameterTypes>(params)...);
        };

        return addMethodInternal(std::move(name), std::move(function));
    }

    template<typename ReturnType, typename T, typename... ParameterTypes>
    MethodWrapper& addMethod(std::string name, ReturnType(T::*method)(ParameterTypes...) const, T& instance) {
        std::function<ReturnType(ParameterTypes...)> function = [&instance, method](ParameterTypes&&... params) -> ReturnType {
            return (instance.*method)(std::forward<ParameterTypes>(params)...);
        };

        return addMethodInternal(std::move(name), std::move(function));
    }

    MethodWrapper& getMethod(const std::string& method_name) { return methods_.at(method_name); }

    std::vector<std::string> getAllMethods() const {
        std::vector<std::string> all_methods;
        all_methods.reserve(methods_.size());

        for (auto& method : methods_) {
            all_methods.emplace_back(method.first);
        }

        return all_methods;
    }

    void removeMethod(const std::string& name) {
        {
            std::lock_guard<std::mutex> lock(methods_mutex_);
            methods_.erase(name);
        }
    }

    std::string invoke(const std::string& name, const std::string& parameters) {
        auto method = methods_.find(name);
        if (method == methods_.end()) {
            // Invalid method
            return std::string("Method Not Found.");
        }

        return { method->second(parameters) };
    }

private:
    template<typename ReturnType, typename... ParameterTypes>
    MethodWrapper& addMethodInternal(std::string name, std::function<ReturnType(ParameterTypes...)> method) {
        return addMethodInternal(std::move(name), std::move(method), std::index_sequence_for<ParameterTypes...> {});
    }

    template<typename... ParameterTypes>
    MethodWrapper& addMethodInternal(std::string name, std::function<void(ParameterTypes...)> method) {
        std::function<std::string(ParameterTypes...)> returnMethod = [method](ParameterTypes&&... params) -> std::string {
            method(std::forward<ParameterTypes>(params)...);

            return std::string("");
        };

        return addMethodInternal(std::move(name), std::move(returnMethod), std::index_sequence_for < ParameterTypes... > {});
    }

    template<typename ReturnType, typename... ParameterTypes, std::size_t... index>
    MethodWrapper& addMethodInternal(std::string name, std::function<ReturnType(ParameterTypes...)> method, std::index_sequence<index...>) {
        MethodWrapper::Method realMethod = [method](const std::string& params) -> std::string {
            return method(params);
        };

        return addMethod(std::move(name), std::move(realMethod));
    }

private:
    std::unordered_map<std::string, MethodWrapper> methods_;

    std::mutex methods_mutex_;
};

} /* end namespace jsonrpc */

#endif /* __JSONRPC_DISPATCHER_HPP__ */
