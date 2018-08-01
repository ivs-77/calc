# ПО для развертывания проекта
1. Клиент github: `yum install git`
1. CMake для сборки: `yum install cmake`
1. Компиляторы C и C++:
`yum install gcc
yum install gcc-c++`
1. Библиотеки PostgreSQL: 
`yum install postgresql-devel`
1. Опционально netcat в качестве простого клиента для работы с сервисом calc: 
`yum install nc`
# Развертывание проекта
1. Выбрать папку, в которую будет загружен проект
1. В выбранной папке выполнить команду 
`git clone https://github.com/ivs-77/calc`
1. После успешного выполнения команды будет создана папка calc с подпапками src, sql и test.
