#ifndef TESTSTORE_HPP
#define TESTSTORE_HPP

#include "test/Common.hpp"

class TestStore:public QObject
{
    Q_OBJECT

private slots:
    void testVariantToString();
    void test();

};


#endif
// TESTSTORE_HPP
