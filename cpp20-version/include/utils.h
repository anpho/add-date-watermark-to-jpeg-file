#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

/**
 * @brief 简单的测试框架
 */
class SimpleTestFramework {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string errorMessage;
    };

    static void runTest(
        const std::string& testName, 
        std::function<bool()> testFunc
    ) {
        std::cout << "Running test: " << testName << " ... ";
        
        try {
            bool result = testFunc();
            if (result) {
                std::cout << "PASS" << std::endl;
                s_results.push_back({testName, true, ""});
            } else {
                std::cout << "FAIL" << std::endl;
                s_results.push_back({testName, false, "Test function returned false"});
            }
        } catch (const std::exception& e) {
            std::cout << "FAIL" << std::endl;
            s_results.push_back({testName, false, std::string("Exception: ") + e.what()});
        } catch (...) {
            std::cout << "FAIL" << std::endl;
            s_results.push_back({testName, false, "Unknown exception"});
        }
    }

    static void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        int passed = 0;
        for (const auto& result : s_results) {
            if (result.passed) {
                passed++;
            } else {
                std::cout << "FAILED: " << result.testName << " - " << result.errorMessage << std::endl;
            }
        }
        std::cout << "Passed: " << passed << "/" << s_results.size() << std::endl;
    }

private:
    static std::vector<TestResult> s_results;
};

std::vector<SimpleTestFramework::TestResult> SimpleTestFramework::s_results;