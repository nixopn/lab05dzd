#Создайте CMakeList.txt для библиотеки banking.
#Создайте модульные тесты на классы Transaction и Account.
#Используйте mock-объекты.
#Покрытие кода должно составлять 100%.
#Настройте сборочную процедуру на TravisCI.
#Настройте Coveralls.io.


`````sh
git clone https://github.com/tp-labs/lab05
`````

`````sh
Cloning into 'lab05'...
remote: Enumerating objects: 137, done.
remote: Counting objects: 100% (25/25), done.
remote: Compressing objects: 100% (9/9), done.
remote: Total 137 (delta 18), reused 16 (delta 16), pack-reused 112
Receiving objects: 100% (137/137), 918.92 KiB | 478.00 KiB/s, done.
Resolving deltas: 100% (60/60), done.
`````

1. Создайте CMakeList.txt для библиотеки banking.
`````sh
	cat > CMakeLists.txt <<EOF
	cmake_minimum_required(VERSION 3.4)
	project(Test_banking)
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)

	option(BUILD_TESTS "Build tests" OFF)

	if(BUILD_TESTS)
	  add_compile_options(--coverage)
	endif()

	add_library(banking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp)
	target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/banking)
	target_link_libraries(banking gcov)

	if(BUILD_TESTS)
	  enable_testing()
	  add_subdirectory(third-party/gtest)
	  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
	  add_executable(check ${BANKING_TEST_SOURCES})
	  target_link_libraries(check banking gtest_main gmock_main)
	  add_test(NAME check COMMAND check)
	endif()
	EOF
`````

2. Создайте модульные тесты на классы Transaction и Account.

`````sh
cd tests

	cat > tests.cpp << EOF
	#include <gtest/gtest.h>
	#include <gmock/gmock.h>

	#include "Account.h"
	#include "Transaction.h"

	class AccountMock : public Account {
	public:
		AccountMock(int id, int balance) : Account(id, balance) {}
		MOCK_CONST_METHOD0(GetBalance, int());
		MOCK_METHOD1(ChangeBalance, void(int diff));
		MOCK_METHOD0(Lock, void());
		MOCK_METHOD0(Unlock, void());
	};
	class TransactionMock : public Transaction {
	public:
		MOCK_METHOD3(Make, bool(Account& from, Account& to, int sum));
	};

	TEST(Account, Mock) {
		AccountMock acc(1, 100);
		EXPECT_CALL(acc, GetBalance()).Times(1);
		EXPECT_CALL(acc, ChangeBalance(testing::_)).Times(2);
		EXPECT_CALL(acc, Lock()).Times(2);
		EXPECT_CALL(acc, Unlock()).Times(1);
		acc.GetBalance();
		acc.ChangeBalance(100); // throw
		acc.Lock();
		acc.ChangeBalance(100);
		acc.Lock(); // throw
		acc.Unlock();
	}

	TEST(Account, SimpleTest) {
		Account acc(1, 100);
		EXPECT_EQ(acc.id(), 1);
		EXPECT_EQ(acc.GetBalance(), 100);
		EXPECT_THROW(acc.ChangeBalance(100), std::runtime_error);
		EXPECT_NO_THROW(acc.Lock());
		acc.ChangeBalance(100);
		EXPECT_EQ(acc.GetBalance(), 200);
		EXPECT_THROW(acc.Lock(), std::runtime_error);
	}

	TEST(Transaction, Mock) {
		TransactionMock tr;
		Account ac1(1, 50);
		Account ac2(2, 500);
		EXPECT_CALL(tr, Make(testing::_, testing::_, testing::_))
		.Times(6);
		tr.set_fee(100);
		tr.Make(ac1, ac2, 199);
		tr.Make(ac2, ac1, 500);
		tr.Make(ac2, ac1, 300);
		tr.Make(ac1, ac1, 0); // throw
		tr.Make(ac1, ac2, -1); // throw
		tr.Make(ac1, ac2, 99); // throw
	}

	TEST(Transaction, SimpleTest) {
		Transaction tr;
		Account ac1(1, 50);
		Account ac2(2, 500);
		tr.set_fee(100);
		EXPECT_EQ(tr.fee(), 100);
		EXPECT_THROW(tr.Make(ac1, ac1, 0), std::logic_error);
		EXPECT_THROW(tr.Make(ac1, ac2, -1), std::invalid_argument);
		EXPECT_THROW(tr.Make(ac1, ac2, 99), std::logic_error);
		EXPECT_FALSE(tr.Make(ac1, ac2, 199));
		EXPECT_FALSE(tr.Make(ac2, ac1, 500));
		EXPECT_TRUE(tr.Make(ac2, ac1, 300));
	}

	EOF
	
	git submodule add https://github.com/google/googletest third-party/gtest
	cd third-party/gtest && git checkout release-1.8.1 && cd ../..
`````

Вывод:

`````sh
Cloning into '/home/nixopn/nixopn/workspace/lab05/third-party/gtest'...
remote: Enumerating objects: 27501, done.
remote: Counting objects: 100% (39/39), done.
remote: Compressing objects: 100% (24/24), done.
remote: Total 27501 (delta 16), reused 29 (delta 10), pack-reused 27462
Receiving objects: 100% (27501/27501), 12.72 MiB | 1.24 MiB/s, done.
Resolving deltas: 100% (20429/20429), done.

Note: switching to 'release-1.8.1'.

You are in 'detached HEAD' state. You can look around, make experimental
changes and commit them, and you can discard any commits you make in this
state without impacting any branches by switching back to a branch.

If you want to create a new branch to retain commits you create, you may
do so (now or later) by using -c with the switch command. Example:

  git switch -c <new-branch-name>

Or undo this operation with:

  git switch -

Turn off this advice by setting config variable advice.detachedHead to false

HEAD is now at 2fe3bd99 Merge pull request #1433 from dsacre/fix-clang-warnings
`````

3. Сделаем push в репозиторий

`````sh
git remote remove origin
git remote add origin https://github.com/${GITHUB_USERNAME}/lab05dzd
git add .
git branch -M master
git commit -m"getting started"
git push origin master
`````

`````sh
[master 15aade6] getting started
 7 files changed, 103 insertions(+), 1 deletion(-)
 create mode 100644 .gitmodules
 create mode 100644 CMakeLists.txt
 delete mode 100644 banking/CMakeList.txt
 delete mode 100644 preview.png
 create mode 100644 tests/tests.cpp
 create mode 160000 third-party/gtest
 create mode 100644 "\320\222\320\272\320\273\321\216\321\207\320\260\320\265\320\274 \321\200\320\265\320\277\320\276\320\267\320\270\321\202\320\276\321\200\320\270\320\271 \320\262 coverals.png"
 
 Username for 'https://github.com': nixopn
Password for 'https://nixopn@github.com':
Enumerating objects: 146, done.
Counting objects: 100% (146/146), done.
Delta compression using up to 8 threads
Compressing objects: 100% (81/81), done.
Writing objects: 100% (146/146), 1.25 MiB | 2.82 MiB/s, done.
Total 146 (delta 61), reused 137 (delta 60), pack-reused 0
remote: Resolving deltas: 100% (61/61), done.
To https://github.com/nixopn/lab05dzd
 * [new branch]      master -> master
`````

4. Создаём workflow

`````sh
name: cmakeoee

on:
  push:
    branches: [master]
  pull_request:
    branches: [master]

jobs:
  build_Linux:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v3

    - name: Adding gtest
      run: git clone https://github.com/google/googletest.git third-party/gtest -b release-1.11.0

    - name: Install lcov
      run: sudo apt-get install -y lcov

    - name: Config banking with tests
      run: cmake -H. -B ${{github.workspace}}/build -DBUILD_TESTS=ON -DCMAKE_CXX_FLAGS='--coverage'

    - name: Build banking
      run: cmake --build ${{github.workspace}}/build

    - name: Run tests
      run: |
        build/check
        lcov --directory . --capture --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage.info
        lcov --remove coverage.info '${{github.workspace}}/third-party/gtest/*' --output-file coverage.info
        lcov --list coverage.info

    - name: Coveralls
      uses: coverallsapp/github-action@master
      with:
        github-token: ${{ secrets.GITHUB_TOKEN }}
        path-to-lcov: ${{ github.workspace }}/coverage.info
`````

[![Coverage Status](https://coveralls.io/repos/github/nixopn/lab05dzd/badge.svg?branch=master)](https://coveralls.io/github/nixopn/lab05dzd?branch=master)
