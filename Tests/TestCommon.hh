// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#define ASSERT_EQ_VECTOR(l, r, litExpr, ritExpr)                                                   \
    {                                                                                              \
        ASSERT_EQ((l).size(), (r).size());                                                         \
        auto lit = (l).begin(), lend = (l).end();                                                  \
        auto rit = (r).begin(), rend = (r).end();                                                  \
        for (; lit != lend && rit != rend; ++lit, ++rit) ASSERT_EQ((litExpr), (ritExpr));          \
    }
