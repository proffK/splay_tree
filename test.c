/*
    This file is part a my splay tree library :)
    Copyright (C) 2016  Kireev Klim

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    My email: proffk@linux.com
*/

#include <splay_tree.h>
#include <stdio.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>

#define TEST_CHECK(cond, num, msg) \
        do {\
                if (!(cond)) {\
                        printf("Test %d %s failed.\n", (num), (msg));\
                        fflush(stdout);\
                } else {\
                        printf("Test %d %s succeed.\n", (num), (msg));\
                        fflush(stdout);\
                }\
        } while (0)

enum tests {
        CR_TEST_0,
        CR_TEST_1,
        CR_TEST_2,
        DL_TEST_0,
        DL_TEST_1,
        DL_TEST_2,
        INS_TEST_0,
        INS_TEST_1,
        INS_TEST_2,
        INS_TEST_3,
        FIND_TEST_0,
        FIND_TEST_1,
        FIND_TEST_2,
        FIND_TEST_3,
        REM_TEST_0,
        REM_TEST_1,
        REM_TEST_2,
        REM_TEST_3,
        FOR_TEST_0,
        FOR_TEST_1,
        FOR_TEST_2
};

node_key test_get_key(node_elem elem);

int callback(splay_tree* tr, node_elem* elem, node_key* key, void* data);

node_key test_get_key(node_elem elem) 
{
        return elem;
}

static int cr_del_test()
{
        struct rlimit old_limit;
        struct rlimit new_limit;

        splay_tree* tr = NULL;

        getrlimit(RLIMIT_AS, &old_limit);
        new_limit.rlim_cur = 2;
        new_limit.rlim_max = old_limit.rlim_max;
        setrlimit(RLIMIT_AS, &new_limit);

        tr = splay_tree_create(test_get_key);

        TEST_CHECK(tr == NULL, CR_TEST_0, "no memory for creation"); 

        setrlimit(RLIMIT_AS, &old_limit);

        tr = splay_tree_create(NULL);
        
        TEST_CHECK(tr == NULL, CR_TEST_0, "incorrect get_key"); 

        tr = splay_tree_create(test_get_key);

        TEST_CHECK(tr != NULL, CR_TEST_2, "correct creation"); 

        TEST_CHECK(splay_tree_delete(NULL) != 0, DL_TEST_0,
                        "incorrect delete addres");

        TEST_CHECK(splay_tree_delete(tr) == 0, DL_TEST_1,
                        "correct deletion");

        return 0;
}

static int ins_find_test()
{
//        struct rlimit old_limit;
//        struct rlimit new_limit;
        int ret = 0;

        splay_tree* tr = NULL;

        tr = splay_tree_create(test_get_key);
        TEST_CHECK(tr != NULL, INS_TEST_0, "correct creation"); 

        ret = splay_tree_insert(NULL, 0);
        TEST_CHECK((ret == -1 || errno == EINVAL), INS_TEST_1,
                   "insert NULL pointer");

        ret = splay_tree_find(tr, 1);
        TEST_CHECK(ret == NULL_NELEM, FIND_TEST_3, 
                   "Search in empty tree");

        ret = splay_tree_for_each_elem(tr, callback, NULL);
        TEST_CHECK(ret == -1, FOR_TEST_1, 
                   "For each in empty tree");

        ret = splay_tree_remove(tr, 1);
        TEST_CHECK(ret == -1, REM_TEST_0,
                        "Remove unfounded elem");

        ret = splay_tree_insert(tr, 5);
        TEST_CHECK(ret == 0, INS_TEST_2,
                   "correct root insertion");

        ret = splay_tree_insert(tr, 6);
        ret = splay_tree_insert(tr, 7);
        ret = splay_tree_insert(tr, 8);
        ret = splay_tree_insert(tr, 9);
        ret = splay_tree_insert(tr, 1);
        ret = splay_tree_insert(tr, 5);
        ret = splay_tree_insert(tr, 4);
        ret = splay_tree_insert(tr, 2);
        ret = splay_tree_insert(tr, 2);
        ret = splay_tree_insert(tr, 3);
        ret = splay_tree_insert(tr, 2);
        ret = splay_tree_insert(tr, 1);
        ret = splay_tree_insert(tr, 1);

        TEST_CHECK(ret == 0, INS_TEST_3,
                   "correct non-root insertion");
        
        ret = splay_tree_find(NULL, 1);

        TEST_CHECK(ret == ERR_NELEM, FIND_TEST_1,
                   "correct NULL tree search");
        ret = splay_tree_find(tr, 1);

        TEST_CHECK(ret == 1, FIND_TEST_1,
                   "correct tree search");

        ret = splay_tree_remove(tr, 1);
        TEST_CHECK(ret == 0, REM_TEST_1, "Correct remove");

        ret = splay_tree_find(tr, 10);

        TEST_CHECK(ret == NULL_NELEM, FIND_TEST_2,
                   "correct fail tree search");

        ret = splay_tree_delete(tr);

        TEST_CHECK(ret == 0, DL_TEST_2,
                   "correct full tree deletion");

        tr = splay_tree_create(test_get_key);
        ret = splay_tree_insert(tr, 2);
        ret = splay_tree_insert(tr, 3);
        ret = splay_tree_insert(tr, 4);
        ret = splay_tree_insert(tr, 5);
        ret = splay_tree_insert(tr, 6);
        ret = splay_tree_insert(tr, 7);
        ret = splay_tree_insert(tr, 8);
        ret = splay_tree_remove(tr, 8);
        ret = splay_tree_insert(tr, 8);
        ret = splay_tree_insert(tr, 1);
        ret = splay_tree_insert(tr, 5);
        ret = splay_tree_remove(tr, 5);

        ret = splay_tree_for_each_elem(NULL, callback, NULL);
        TEST_CHECK(ret == -1, FOR_TEST_0, "For each NULL");
        
        ret = splay_tree_for_each_elem(tr, NULL, NULL);
        TEST_CHECK(ret == -1, FOR_TEST_1, "For each no callback");

        ret = splay_tree_for_each_elem(tr, callback, NULL);
        TEST_CHECK(ret == 0, FOR_TEST_2, "Correct for each");

        ret = splay_tree_delete(tr);

//        getrlimit(RLIMIT_AS, &old_limit);
//        new_limit.rlim_cur = 4;
//        new_limit.rlim_max = old_limit.rlim_max;
//        setrlimit(RLIMIT_AS, &new_limit);
//
//        setrlimit(RLIMIT_AS, &old_limit);

        return ret;
}

                        

int main() {

        int test_res = 0;

        test_res = cr_del_test();

        if (test_res == 0) {
                printf("All create and delete test passed\n");
        } else {
                printf("Create and delete test %d failed\n", test_res);
        }
        
        test_res = ins_find_test();

        if (test_res == 0) {
                printf("All insert and find test passed\n");
        } else {
                printf("Insert and find test %d failed\n", test_res);
        }
 
        return test_res;
}

int callback(splay_tree* tr, node_elem* elem, node_key* key, void* data)
{
        printf("elem: %d\n", *elem);
        return 0;
}

