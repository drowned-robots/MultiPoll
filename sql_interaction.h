/* 
 * File:   sql_interaction.h
 * Author: u1
 *
 * Created on November 24, 2014, 12:15 PM
 */

#ifndef SQL_INTERACTION_H
#define	SQL_INTERACTION_H

#ifdef	__cplusplus
extern "C" {
#endif




#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#define N 13

	SQLHENV	V_OD_Env; // handle ODBC environment
	long V_OD_erg;	// f() result 
	SQLHDBC	V_OD_hdbc; // handle connection
	char V_OD_stat[10]; // status SQL
	SQLINTEGER V_OD_err,V_OD_rowanz,V_OD_id;
	SQLSMALLINT V_OD_mlen,V_OD_colanz;
	char V_OD_msg[200], V_OD_buffer[200];
	//
	int createquery(char * s, char * query); // chars[] > sql query
	int sendquery(char * s); // sql query > sql server


	int createquery(char * s, char * query)
	{
		char vals[N][5001];
		char sqlfields[N][20] = {"conference", "fio", "subjectrf", "organization", "occupation", "academicdegree", "telno", "email", "participationtype", "exp", "intr", "reptheme", "commentsquestions"};
		int sqlsize[N] = {500,150,100,250,250,250,50,50,50,5000,5000,5000,5000};
		char phpfields[N][15] = {"conf=", "fullname=", "sub=", "organization=", "occupation=", "degree=", "phone=", "email=", "type=", "exp=", "intr=", "subject=", "comment="};
		memset(vals, 0, sizeof(vals));
		char q1[1000], q2[25000];
		memset(q1, 0, sizeof(q1));
		memset(q2, 0, sizeof(q2));
		strcat(q1, "insert into participants(");
		strcat(q2, ") values (");
		int i, j, j1, itemcount=0;
		int s_len = strlen(s);
		char * ptr;
		for(i=0;i<N;i++)
		{
			ptr = strcasestr(s, phpfields[i]);
			if(ptr==NULL)
			{
				if((i<2)||(i==3)||(i==4)||(i==7)||(i==8)||(i==9)||(i==10))
					return -1;
				continue;
			}
			j1=0;
			j = ptr - s + strlen(phpfields[i]);
			while((j<s_len)&&(j1<sqlsize[i]))
			{
				if(j<s_len-1)
					if(s[j]=='&')
					{
						j++;
						break;
					}
				vals[i][j1]=s[j];
				j1++;
				j++;
			}
			if(j1<2)
			{
				if((i<2)||(i==3)||(i==4)||(i==7)||(i==8)||(i==9)||(i==10))
					return -1;
				continue;
			}
			if(itemcount!=0)
				strcat(q1, ", ");
			strcat(q1, sqlfields[i]);
			if(itemcount!=0)
				strcat(q2, ", N'");
			else
				strcat(q2, "N'");
			strcat(q2, vals[i]);
			strcat(q2, "'");
			itemcount++;
			// printf("%s\t%s\n", &phpfields[i], &vals[i]);
			// printf("%s\t%s\n", q1, q2);
		}
		strcat(query, q1);
		strcat(query, q2);
		strcat(query, ")");
		printf("%s\n", query);
		return 0;
	}

	int sendquery(char * s)
	{
		SQLHSTMT V_OD_hstmt;
		// allocate Environment handle and register version 
		V_OD_erg=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&V_OD_Env);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
			return -1;

		V_OD_erg=SQLSetEnvAttr(V_OD_Env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
			return -1;
		}
		// allocate connection handle, set timeout
		V_OD_erg = SQLAllocHandle(SQL_HANDLE_DBC, V_OD_Env, &V_OD_hdbc); 
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
			return -1;
		}
		SQLSetConnectAttr(V_OD_hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER *)5, 0);
		//----------------------------------------------Connect to the SQLTEST
		V_OD_erg = SQLConnect(V_OD_hdbc, (SQLCHAR*) "v03-sql", SQL_NTS,
				(SQLCHAR*) "uid500", SQL_NTS,
				(SQLCHAR*) "g0vvyfkFx", SQL_NTS);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, V_OD_stat, &V_OD_err,V_OD_msg,100,&V_OD_mlen);
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
			return -1;
		}
		V_OD_erg=SQLAllocHandle(SQL_HANDLE_STMT, V_OD_hdbc, &V_OD_hstmt);
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
			SQLDisconnect(V_OD_hdbc);
			SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
			return -1;
		}
		SQLBindCol(V_OD_hstmt,1,SQL_C_CHAR, &V_OD_buffer,150,&V_OD_err);
		SQLBindCol(V_OD_hstmt,2,SQL_C_ULONG,&V_OD_id,150,&V_OD_err);
		//-----------------------------------------------QUERY HERE
		V_OD_erg=SQLExecDirect(V_OD_hstmt, (SQLCHAR *)s, SQL_NTS); 
		if ((V_OD_erg != SQL_SUCCESS) && (V_OD_erg != SQL_SUCCESS_WITH_INFO))
		{
			SQLGetDiagRec(SQL_HANDLE_DBC, V_OD_hdbc,1, V_OD_stat,&V_OD_err,V_OD_msg,100,&V_OD_mlen);
			SQLFreeHandle(SQL_HANDLE_STMT,V_OD_hstmt);
			SQLDisconnect(V_OD_hdbc);
			SQLFreeHandle(SQL_HANDLE_DBC,V_OD_hdbc);
			SQLFreeHandle(SQL_HANDLE_ENV, V_OD_Env);
			return -1;
		}
		return 0;
	}


#ifdef	__cplusplus
}
#endif

#endif	/* SQL_INTERACTION_H */

//some comment here
