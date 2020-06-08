/*
 * signletone.h
 *
 *  Created on: 10 May 2020
 *      Author: yonathan
 */

#ifndef SIGNLETONE_H_
#define SIGNLETONE_H_

#include<mutex>



template<class T>
class singletone
{
public:
	static T* getinstance()
	{
		if(!m_singletone)
		{
			std::lock_guard<std::mutex>m(m_mutex);
			if(!m_singletone)
			{
				m_singletone = new T;
			}
		}
	 return m_singletone;
    }
private:
	singletone();
	~singletone();
	static T* m_singletone;
	static  std::mutex m_mutex;
};

template<class T>T* singletone<T>::m_singletone = nullptr;
template<class T>std::mutex singletone<T>::m_mutex;

#endif /* SIGNLETONE_H_ */
