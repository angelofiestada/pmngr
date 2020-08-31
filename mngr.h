#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include "encrypt.h"
#include "sqlite3.h"
#include <stdlib.h>
#include "database.h"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <algorithm>

const std::string GLOBAL_DATABASE = "act.db";
using Record = std::vector<std::string>;
using Records = std::vector<Record>;
using namespace std;

int insert(string sql);
int insertTable(string sql);
Records gk(int md);
void menu();
string getDate()
{

    time_t now = time(0);
    char *dt = ctime(&now);
    tm *gmtm = gmtime(&now);
    dt = asctime(gmtm);
    return dt;
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    printf("%s", argv[0]);
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("______Opp%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
int sqlite3_user_authenticate(
    sqlite3 *db,           /* The database connection */
    const char *zUsername, /* Username */
    const char *aPW,       /* Password or credentials */
    int nPW                /* Number of bytes in aPW[] */
);
int login_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
    Records *records = static_cast<Records *>(p_data);
    try
    {
        records->emplace_back(p_fields, p_fields + num_fields);
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
Records select(const char *stmt)
{
    Records records;
    char *errmsg;
    sqlite3 *db;
    if (sqlite3_open(GLOBAL_DATABASE.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Could not open database.\n";
    }
    int ret = sqlite3_exec(db, stmt, login_callback, &records, &errmsg);
    if (ret != SQLITE_OK)
    {
        std::cerr << "Error in select statement " << stmt << "[" << errmsg << "]\n";
    }
    else
    {
        // std::cerr << records.size() << " records returned.\n";
        printf("\ndone");
    }
    sqlite3_close(db);
    return records;
}
int insert(const char *sql)
{
    int successfull = 0;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    /* Open database */
    rc = sqlite3_open(GLOBAL_DATABASE.c_str(), &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        successfull = 1;
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
    }

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        successfull = 1;
    }
    else
    {
        fprintf(stdout, "\nRecords created successfully");
    }
    sqlite3_close(db);
    return successfull;
}
void printHeader()
{
    printf("\n ____                                         __  ");
    printf("\n|  _ \\                                       |  | ");
    printf("\n| |_) \\____  ___  ___ __    __ ___   _  _  __|  | ");
    printf("\n|    // _  |/ __|/ __|\\ \\/\\/ // _ \\ | `'_|/  _  |");
    printf("\n|  .-| (_| |\\__ \\__  \\ \\    /| (_) ||  /  | (_| |");
    printf("\n|__|  \\___,||___/|___/  \\/\\/  \\___/ |__|  \\_____|");
    printf("\n");
}
string gmk(size_t length)
{
    const char *charmap = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~";
    const size_t charmapLength = strlen(charmap);
    auto generator = [&]() { return charmap[rand() % charmapLength]; };
    string result;
    result.reserve(length);
    generate_n(back_inserter(result), length, generator);
    return result;
}
bool login()
{

    string _psswrd, e_password, get_password_sql, insert_password_sql,
        user_password_sql, e_user_password;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    const char *data = "Callback function called";
    std::string date = getDate();
    bool successful = false;

    rc = sqlite3_open(GLOBAL_DATABASE.c_str(), &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }

    get_password_sql = "SELECT pw FROM password";
    user_password_sql = "SELECT pw FROM password ORDER BY date DESC LIMIT 1;";
    Records records = select(get_password_sql.c_str());
    if (records.size() != 0)
    {
        string key = decrypt(gk(1)[0][0], gk(0)[0][0]);
        printf("\n----------Login----------");
        printf("\nPassword:  ");
        cin >> _psswrd;
        e_password = encrypt(_psswrd, key);
        e_user_password = select(user_password_sql.c_str())[0][0];
        printf("OLD PASSWORD%s", decrypt(e_user_password, key).c_str());
        if (int match = e_password.compare(e_user_password) == 0)
            successful = true;

        else
        {
            while (successful == false)
            {

                system("clear");
                printf("\n----------Login----------");
                printf("\nIncorrect Password. Please try again..\n");
                printf("\nPassword:  ");
                cin >> _psswrd;
                e_password = encrypt(_psswrd, key);
                if (int match = e_password.compare(e_user_password) == 0)
                    successful = true;
            }
        }
    }
    else
    {
        string mk = gk(0)[0][0];
        string create_password;
        string confirm_create_password;
        printf("\n----------Create Password----------");
        printf("\nEnter Password: ");
        cin >> create_password;
        printf("\nConfirm Password: ");
        cin >> confirm_create_password;

        if (int match = create_password.compare(confirm_create_password) == 0)
        {
            successful = true;
        }
        else
        {
            while (!successful)
            {
                printf("\nPassword doesn't match\nPlease try again..\n");
                printf("\nEnter Password: ");
                cin >> create_password;
                printf("\nConfirm Password: ");
                cin >> confirm_create_password;

                if (int match = create_password.compare(confirm_create_password) == 0)
                    successful = true;
            }
        }

        //Insert Password to the database
        string date = getDate();
        string e_create_password = encrypt(create_password, mk);
        string e_confirm_create_password = encrypt(confirm_create_password, mk);

        string sql = "INSERT INTO password (pw, date)"
                     "VALUES('" +
                     e_confirm_create_password + "','" + date + "');";
        if (insert(sql.c_str()) == 0)
        {

            successful = false;
            string p_key1, p_key2;
            //Create Private Key
            printf("\n----------Create Private Key----------");
            printf("\n\nThis key will be used to encrypt your passwrds");
            printf("\n\tStore it in a secure place\n\n");
            printf("\n\nEnter your key: ");
            cin >> p_key1;
            printf("\nConfirm your key: ");
            cin >> p_key2;

            if (int match = p_key1.compare(p_key2) == 0)
            {
                successful = true;
            }
            else
            {
                while (!successful)
                {
                    printf("\n----------Create Private Key----------");
                    printf("\n\nKey did not Match.\nPlease try again...");
                    printf("\n\nEnter your key: ");
                    string p_key1;
                    cin >> p_key1;
                    printf("\nConfirm your key: ");
                    string p_key2;
                    cin >> p_key2;
                    if (int match = p_key1.compare(p_key2) == 0)
                    {
                        successful = true;
                    }
                }
            }

            //Insert Private key to the database
            string e_p_key2 = encrypt(p_key2, mk);
            string sql = "INSERT INTO privatekey (key, date)"
                         "VALUES('" +
                         e_p_key2 + "','" + date + "');";
            string p_sql = "SELECT * FROM password";
            string _psswrd = select(p_sql.c_str())[0][0];
            string d_psswrd = decrypt(_psswrd, mk);
            string update_sql = "UPDATE password SET pw='" + encrypt(d_psswrd, p_key1) + "' WHERE pw='" + _psswrd.c_str() + "'";
            if (insert(sql.c_str()) == 0 && insert(update_sql.c_str()) == 0)
            {
                successful = true;
            }
        }
    }
    return successful;
}
Records gk(int md)
{
    string mode = (md == 0) ? "ASC" : "DESC";
    Records records;
    string key_sql;
    key_sql = "SELECT key FROM privatekey ORDER BY date " + mode + " LIMIT 1;";
    records = select(key_sql.c_str());
    return records;
}
void addRecord()
{
    Records record;
    string _psswrd, account_name, username, note, e_password, _ccnt_nm,
        e_username, e_note, sql, key;
    key = decrypt(gk(1)[0][0], gk(0)[0][0]);
    printf("PALIN key ==== %s", key.c_str());
    printf("\nAccount name: ");
    cin >> account_name;
    sql = "SELECT account_name FROM account WHERE account_name = '" + encrypt(account_name, key) + "';";
    if (select(sql.c_str()).size() == 0)
    {
        printf("\nUsername: ");
        cin >> username;
        printf("\nPassword: ");
        cin >> _psswrd;
        cin.ignore();
        cout << "\nNotes[Optional]: ";
        getline(cin, note);

        if (note.empty())
        {
            e_note = "";
        }
        else
        {
            e_note = encrypt(note, key);
        }
        _ccnt_nm = encrypt(account_name, key);
        e_password = encrypt(_psswrd, key);
        e_username = encrypt(username, key);
        sql = "INSERT INTO account (account_name, username, password, note)"
              "VALUES('" +
              _ccnt_nm + "','" + e_username + "','" + e_password + "','" + e_note +
              "');";
        insert(sql.c_str());
    }
    else
    {
        printf("\nEntered Account name already exist");
        menu();
    }
    menu();
}
void selectRecord(const char *sql)
{
    Records records = select(sql);
    string key = decrypt(gk(1)[0][0], gk(0)[0][0]);
    for (auto record : records)
    {
        printf("\nAccount name: %s", decrypt(record[0], key).c_str());
        printf("\nUsername:     %s", decrypt(record[1], key).c_str());
        printf("\nPassword:     %s", decrypt(record[2], key).c_str());
        printf("\nNote:         %s", decrypt(record[3], key).c_str());
        printf("\n\n");
    }
    menu();
}
void printMenu()
{
    printf("----------List of Commands----------");
    printf("\nanr - Add New Record");
    printf("\nsr  - Select Record");
    printf("\nsar - Select All Record");
    printf("\ncp  - Change Password");
    printf("\nck  - Change Key");
    printf("\nexit- Exit\n");
}
void cK()
{
    Records records;
    vector<string> accounts;
    string nw_prvt_ky, cnfrm_prvt_ky, sr_psswrd, d_ld_ky, sql, _cnfrm_prvt_ky,
        _ccnt_nm, e_username, e_password, e_note, d_psswrd;
    string mk_sql = "SELECT key FROM privatekey ORDER BY date ASC LIMIT 1;";
    string password_sql = "SELECT pw FROM password ORDER BY date DESC LIMIT 1;";
    string mk = select(mk_sql.c_str())[0][0];

    printf("\n-----------Change Private Key--------");
    printf("\n New Private Key: ");
    cin >> nw_prvt_ky;
    printf("\n Confirm New Private Key: ");
    cin >> cnfrm_prvt_ky;
    printf("\nPassword: ");
    cin >> sr_psswrd;

    std::string date = getDate();

    d_psswrd = select(password_sql.c_str())[0][0];
    d_ld_ky = decrypt(gk(1)[0][0], gk(0)[0][0]);

    printf("%s\n", d_psswrd.c_str());
    printf("%s\n", encrypt(sr_psswrd, d_ld_ky).c_str());

    if (int match = d_psswrd.compare(encrypt(sr_psswrd, d_ld_ky)) == 0 && nw_prvt_ky.compare(cnfrm_prvt_ky) == 0)
    {
        _cnfrm_prvt_ky = encrypt(cnfrm_prvt_ky, mk);
        sql = "INSERT INTO privatekey (key, date)"
              "VALUES('" +
              _cnfrm_prvt_ky + "','" + date + "');";
        if (insert(sql.c_str()) == 0)
        {
            printf("\nKey updated succesfully");
            printf("\nUpdating Databse...");
            string key = decrypt(gk(1)[0][0], gk(0)[0][0]);

            sql = "SELECT * FROM account";
            records = select(sql.c_str());
            for (auto record : records)
            {
                string account_name = decrypt(record[0], d_ld_ky);
                string username = decrypt(record[1], d_ld_ky);
                string password = decrypt(record[2], d_ld_ky);
                string note = decrypt(record[3], d_ld_ky);
                sql = "UPDATE account  SET account_name = '" + encrypt(account_name, key) + "',username = '" + encrypt(username, key) + "', password = '" + encrypt(password, key) + "',note ='" + encrypt(note, key) + "' WHERE account_name = '" + record[0].c_str() + "';";
                insert(sql.c_str());
            }

            sql = "SELECT * FROM password";
            records = select(sql.c_str());

            for (auto record : records)
            {
                string _psswrd = decrypt(record[0], d_ld_ky);
                sql = "UPDATE password  SET  pw= '" + encrypt(_psswrd, key) + "' WHERE pw= '" + record[0].c_str() + "';";
                insert(sql.c_str());
            }
        }
    }
    else
    {
        if (int match = nw_prvt_ky.compare(cnfrm_prvt_ky) != 0)
        {

            printf("\nPivate Key don't match");
        }
        else
        {
            printf("\nYour password is incorrect");
        }
    }
}
void cP()
{
    string old_password, new_password, confirm_password, d_old_password, sql, e_new_password;
    string key = decrypt(gk(1)[0][0], gk(0)[0][0]);
    printf("\n-----------Change your Password--------");
    printf("\n Current password: ");
    cin >> old_password;
    printf("\n New password: ");
    cin >> new_password;
    printf("\n Confirm password: ");
    cin >> confirm_password;
    sql = "SELECT pw FROM password ORDER BY date DESC LIMIT 1;";
    std::string date = getDate();
    d_old_password = select(sql.c_str())[0][0];

    if (int match = new_password.compare(confirm_password) == 0 && d_old_password.compare(encrypt(old_password, key)) == 0)
    {
        printf("ALL SET");
        e_new_password = encrypt(new_password, key);
        sql = "INSERT INTO password (pw, date)"
              "VALUES('" +
              e_new_password + "','" + date + "');";
        if (insert(sql.c_str()) == 0)
        {
            printf("Password updated succesfully");
        }
    }
    else
    {
        if (int match = new_password.compare(confirm_password) != 0)
        {
            printf("\nPassword don't match");
        }
        else
        {
            printf("\nYour old password is incorrect");
        }
    }
}
void menu()
{
    string sql, key;
    string choice;

    printf("\n --> ");
    cin >>
        choice;

    if (choice == "anr")
    {
        addRecord();
    }
    else if (choice == "sr")
    {
        key = decrypt(gk(1)[0][0], gk(0)[0][0]);
        string account_name;
        string _ccnt_nm;
        printf("\nEnter account name: ");
        cin >> account_name;
        _ccnt_nm = encrypt(account_name, key);

        sql = "SELECT account_name, password, username FROM account WHERE account_name='" + _ccnt_nm + "';";
        selectRecord(sql.c_str());
    }
    else if (choice == "sar")
    {
        sql = "SELECT * FROM account";
        selectRecord(sql.c_str());
        printf("---> ");
        cin >> choice;
    }
    else if (choice == "cp")
    {
        cP();
        menu();
    }
    else if (choice == "ck")
    {
        cK();
        menu();
    }
    else if (choice == "Exit" || choice == "exit")
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        printMenu();
        menu();
    }
}
void initialize()
{
    Records records;
    sqlite3 *db;
    char *zErrMsg = 0;
    string sql;
    const char *data = "Callback function called";
    int rc;
    string table1 = "account";
    string table2 = "password";
    string table3 = "privatekey";

    printf("\nInitiliazing...");
    rc = sqlite3_open(GLOBAL_DATABASE.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        printf("\nËrror in creating database");
    }
    else
    {
        printf("\nDatebase available");
    }

    //Create account table
    string act_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name= '" + table1 + "';";
    string psswrd_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name= '" + table2 + "';";
    string act_tbl_sql = "CREATE TABLE 'account' ('account_name'	TEXT NOT NULL,'username'	TEXT NOT NULL,'password'	TEXT NOT NULL,'note'	TEXT,PRIMARY KEY('account_name'));";
    string psswrd_tbl_sql = "CREATE TABLE 'password' ('pw'	TEXT NOT NULL,'date'	TEXT NOT NULL,PRIMARY KEY('pw'));";
    string prvt_ky_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name= '" + table3 + "';";
    string prvt_ky_tbl_sql = "CREATE TABLE 'privatekey' ('key'	TEXT NOT NULL,'date'	TEXT NOT NULL);";

    records = select(act_sql.c_str());
    cerr << records.size();
    if (records.size() == 0)
    {
        select(act_tbl_sql.c_str());
    }

    //Create password table
    records = select(psswrd_sql.c_str());
    if (records.size() == 0)
    {
        select(psswrd_tbl_sql.c_str());
    }

    //Create privatekey table
    records = select(prvt_ky_sql.c_str());
    if (records.size() == 0)
    {
        select(prvt_ky_tbl_sql.c_str());
    }
    string pk_sql = "SELECT key FROM privatekey ORDER BY date DESC LIMIT 1;";
    records = select(pk_sql.c_str());
    if (records.size() == 0)
    {
        string key = gmk(24);
        string sql = "INSERT INTO privatekey (key, date)"
                     "VALUES('" +
                     key + "','" + getDate() + "');";
        insert(sql.c_str());
    }
    sqlite3_close(db);
}
