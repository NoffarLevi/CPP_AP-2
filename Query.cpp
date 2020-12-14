#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <sstream> 
#include <regex>
using namespace std;
/////////////////////////////////////////////////////////
shared_ptr<QueryBase> QueryBase::factory(const string& s)
{
    istringstream is(s);
    string op;
    string s1;
    string s2;
    string e;
    is >> op;
    is >> s1;
    is >> s2;
    is >> e;

    if(s1.empty()){
       return std::shared_ptr<QueryBase>(new WordQuery(op));
    }
    else if( !s1.empty() && !s2.empty() && e.empty()){
        if(op == "AND"){           
            return std::shared_ptr<QueryBase>(new AndQuery(s1,s2));
        }
        else if(op == "OR"){
             return std::shared_ptr<QueryBase>(new OrQuery(s1,s2));

        }
        else if(op == "AD"){
            return std::shared_ptr<QueryBase>(new AdjacentQuery(s1,s2));
        }
        else{
            throw invalid_argument("Unrecognized search \n");
        }
    }
    else{
        throw invalid_argument("Unrecognized search \n");
    }
}

QueryResult AndQuery::eval(const TextQuery& text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    set_intersection(left_result.begin(), left_result.end(),
        right_result.begin(), right_result.end(), 
        inserter(*ret_lines, ret_lines->begin()));
   return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>(left_result.begin(), left_result.end());
    ret_lines->insert(right_result.begin(), right_result.end());
    return QueryResult(rep(), ret_lines, left_result.get_file());
}
QueryResult AdjacentQuery::eval (const TextQuery& text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    auto l_it = left_result.begin();
    auto r_it = right_result.begin();
    int l_prev = 0;
    int r_prev = 0;

    while(l_it != left_result.end() && r_it != right_result.end()){

            if(*l_it < *r_it){
              if(*l_it == *r_it - 1){
                
                  ret_lines->insert(*l_it);
                  ret_lines->insert(*r_it);
                  l_prev = *l_it;
                  r_prev = *r_it;
                  l_it++;
              //    r_it++;                                  
            }else{
                l_it++;
            }   
            }else if(*l_it > *r_it){
                if(*l_it == *r_it + 1){
                  ret_lines->insert(*r_it);
                  ret_lines->insert(*l_it);
                  l_prev = *l_it;
                  r_prev = *r_it;
             //     l_it++;
                  r_it++;
            }else{
                r_it++;
            }   
            }

    }
    return QueryResult(rep(), ret_lines, left_result.get_file());
}
std::ostream &print(std::ostream &os, const QueryResult &qr)
{
    if(qr.sought.find("AD") == string::npos){
        os << "\"" << qr.sought << "\"" << " occurs " << 
        qr.lines->size() << " times:" <<std::endl;

    for (auto num : *qr.lines)
    {
        os << "\t(line " << num + 1 << ") " 
            << *(qr.file->begin() + num) << std::endl;
    }
    }else{
        int v = 0;
        int c = 0;
        int p = 0;
        if(qr.lines->size()%2 == 0){
            v = 0;
            os << "\"" << qr.sought << "\"" << " occurs " << 
            qr.lines->size()/2 << " times:" <<std::endl;

            for(auto num : *qr.lines){
               os << "\t(line " << num + 1 << ") " 
               << *(qr.file->begin() + num) << std::endl;
               p++;
                c++; 
                if(c != 6 && p == 2 && qr.lines->size()>2){
                    os << endl;
                    p=0;
                }               
            }
        }else{
            int c = 0;
            if(qr.lines->size()==3){
             os << "\"" << qr.sought << "\"" << " occurs " << 
            qr.lines->size()/2 +1 << " times:" <<std::endl;   
                for (auto num : *qr.lines)
               {   
                c++;
                os << "\t(line " << num + 1 << ") " 
                << *(qr.file->begin() + num) << std::endl;
               if(c == 2){
                   os << endl;
                   os << "\t(line " << num + 1 << ") " 
                   << *(qr.file->begin() + num) << std::endl;
               }
               }
            }
            
        }



    }
    
    return os;
}
// if(qr.sought.find("AD") != string::npos){
//         os << "\"" << qr.sought << "\"" << " occurs " << 
//         qr.lines->size()/2 << " times:" <<std::endl;

//         int c = 1;

//     for (auto num : *qr.lines)
//     {
//         os << "\t(line " << num + 1 << ") " 
//             << *(qr.file->begin() + num) << std::endl;
//         if(c%2== 0 && ){
//             os << '\n' << std::endl;   
//         }
//         c++;
//     }

//      }else{
//         os << "\"" << qr.sought << "\"" << " occurs " << 
//         qr.lines->size() << " times:" <<std::endl;  
//     for (auto num : *qr.lines)
//     {
//         os << "\t(line " << num + 1 << ") " 
//             << *(qr.file->begin() + num) << std::endl;
//     }
//     }