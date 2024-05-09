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
    AccountMock acc(10, 500);
    EXPECT_CALL(acc, GetBalance()).Times(1);
    EXPECT_CALL(acc, ChangeBalance(testing::_)).Times(2);
    EXPECT_CALL(acc, Lock()).Times(2);
    EXPECT_CALL(acc, Unlock()).Times(1);
    acc.GetBalance();
    acc.ChangeBalance(200); // throw
    acc.Lock();
    acc.ChangeBalance(300);
    acc.Lock(); // throw
    acc.Unlock();
}

TEST(Account, SimpleTest) {
    Account acc(5, 200);
    EXPECT_EQ(acc.id(), 5);
    EXPECT_EQ(acc.GetBalance(), 200);
    EXPECT_THROW(acc.ChangeBalance(200), std::runtime_error);
    EXPECT_NO_THROW(acc.Lock());
    acc.ChangeBalance(300);
    EXPECT_EQ(acc.GetBalance(), 500);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(Transaction, Mock) {
    TransactionMock tr;
    Account ac1(3, 75);
    Account ac2(4, 700);
    EXPECT_CALL(tr, Make(testing::_, testing::_, testing::_))
        .Times(6);
    tr.set_fee(150);
    tr.Make(ac1, ac2, 299);
    tr.Make(ac2, ac1, 700);
    tr.Make(ac2, ac1, 400);
    tr.Make(ac1, ac1, 10); // throw
    tr.Make(ac1, ac2, -10); // throw
    tr.Make(ac1, ac2, 199); // throw
}

TEST(Transaction, SimpleTest) {
    Transaction tr;
    Account ac1(6, 75);
    Account ac2(7, 700);
    tr.set_fee(150);
    EXPECT_EQ(tr.fee(), 150);
    EXPECT_THROW(tr.Make(ac1, ac1, 10), std::logic_error);
    EXPECT_THROW(tr.Make(ac1, ac2, -10), std::invalid_argument);
    EXPECT_THROW(tr.Make(ac1, ac2, 199), std::logic_error);
    EXPECT_FALSE(tr.Make(ac1, ac2, 299));
    EXPECT_FALSE(tr.Make(ac2, ac1, 700));
    EXPECT_TRUE(tr.Make(ac2, ac1, 400));
}

