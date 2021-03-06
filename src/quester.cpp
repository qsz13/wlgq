#include "quester.h"
#include <algorithm>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/bind.hpp>

using namespace boost::algorithm;

static vector<word_position> empty_vector;

quester::quester(query_operator *o)
    :m_operator(o)
{

}

quester::~quester()
{
    //dtor
}

void quester::execute(string queryString)
{
    //parse the query and
    //process operator of AND,OR,etc...

    //simple implement
    stringstream s;
    s.str(queryString);

    string temp;
        s>>temp;

    query result=create_query(temp);
    while (!s.eof())
    {
        s>>temp;
        if (temp=="AND")
        {
            s>>temp;
            result=m_operator->operator_and(result,create_query(temp));
        }
        else if (temp=="OR")
        {
            s>>temp;
            result=m_operator->operator_or(result,create_query(temp));
        }
        else
        {
            result=m_operator->operator_and(result,create_query(temp));
        }
    }

    m_printer.printAll(result);
}

query quester::create_query(string keyword)
{
    vector<word_position> *vec=m_indexer.find_word(keyword);
    if (vec)
        return query(*vec,keyword);
    else
        return query(empty_vector,keyword);

}

void quester::read_documents(vector<path> files)
{
    //TODO:use multithread
    for_each(files.begin(),files.end(),
             boost::bind(boost::mem_fn(&quester::read_single_document),this,_1));
}

void quester::read_single_document(path filename)
{
    boost::filesystem::ifstream s;
    string str;
    
    
    
     //modify by qsz13-------------------------------------------------
        for(string::iterator it = str.begin(); it != str.end();)
    {
        if(ispunct(*it))
        {
            if((*it == '-') && str.size() != 1)
            {
                it++;
            }
            else
            {
                it = str.erase(it);
            }
        }
        else
        {
            *it = tolower(*it);
            it++;
        }
    }

     //----------------------------------------------------------------------
    
    
    
    
    s.open(filename);
    while (!s.eof())
    {
        s>>str;
        word_position pos;
        pos.document_name=filename.filename().generic_string();
        pos.position=s.tellg();

        //this will occur a error in multithread mode
        m_indexer.add_word(str,pos);
//        cout<<str<<endl;

    }
    s.close();
}
