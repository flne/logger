#include <windows.h>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>  
#include <cstdio>
#include <stdio.h>
#include <dirent.h>

using namespace std;

int main() {
    
    string membuf; 
    SYSTEMTIME lt;
    
    ostringstream buffer;
    _mkdir("log"); 

    DIR* dirlog = opendir("log"); 
    if (dirlog == NULL) {
        perror("opendir");
        return -1;
    }

    _chdir("log");


    struct dirent* entry;
    int n;
    int bytes;
    long double newf_size = 0;
    long double size_all;
    string oldf;
    string newf;
    string zH, zM, zS, zMS;
    string file_name;
    string record;
    ofstream file;
    // цикл, который логирует номер каждой итерации
    for (int j = 0; j < 3000; j++) {

        GetLocalTime(&lt);
        
        ostringstream date_stream;

        zH = lt.wHour < 10 ? "0" : "";
        zM = lt.wMinute < 10 ? "0" : "";
        zS = lt.wSecond < 10 ? "0" : "";  
        zMS = lt.wMilliseconds < 100 ? "0" : "";  
        date_stream << lt.wDay << "-"  
            << lt.wMonth << "-" 
            << lt.wYear << " " 
            << zH << lt.wHour << "." 
            << zM << lt.wMinute << "." 
            << zS << lt.wSecond << "." 
            << zMS << lt.wMilliseconds;
    
        buffer << "[" << date_stream.str() << "] Iteration #" << j + 1 << endl;

        file_name = date_stream.str() + ".txt";

        membuf = buffer.str();  
    
        if (membuf.size() > 150)   {

            rewinddir(dirlog); 

            n = 0; 
            size_all = 0; 
            // этот цикл позволит узнать:
            // в переменную oldf - имя самого старого файла
            // в переменную newf - имя самого нового файла
            // в переменную newf_size - вес самого нового файла
            // общий вес всех файлов в папке log в переменную size_all
            while (entry = readdir(dirlog)) { 
                n++;
                if (n <= 2) continue; // пропуск текущего (.) и родительского (..) каталогов
                
                if (n == 3) oldf = entry->d_name; // имя самого старого файла
                newf = entry->d_name; // имя самого нового файла
        
                ifstream infile(entry->d_name);
                infile.seekg(0, ios::end);
                bytes = infile.tellg();
                size_all += newf_size = static_cast<long double>(bytes) / 1024;
            }
            
            // если в папке кроме . и .. ничего нет, или самый новый файл весит больше 50 кб, то
            // создаём новый файл, чтобы запись шла уже в него
            if (n == 2 || newf_size > 50) record = file_name;
            else record = newf;

            file.open(record.c_str(), ios::app);
            
    
            file << membuf;

            buffer.str("");
            file.close();
            if (size_all > 300) 
                remove(oldf.c_str());
        }
    }
    if (buffer.str() != "") {
        file.open(record.c_str(), ios::app);
        // дописываем то что осталось в буфере
        file << membuf;
        file.close();
    }
    closedir(dirlog);
}
