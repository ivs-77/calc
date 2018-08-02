# ПО для развертывания проекта
В минимальной установке CentOS 7 для сборки проекта необходимо дополнительно установить следующее ПО:<br/>
1. Клиент github:<br/>
`yum install git`
1. CMake для сборки:<br/>
`yum install cmake`
1. Компиляторы C и C++:<br/>
`yum install gcc`<br/>
`yum install gcc-c++`
1. Библиотеки PostgreSQL:<br/> 
`yum install postgresql-devel`
1. Опционально netcat в качестве простого клиента для работы с сервисом calc:<br/> 
`yum install nc`
# Развертывание проекта
1. Выбрать папку, в которую будет загружен проект
1. В выбранной папке выполнить команду:<br/> 
`git clone https://github.com/ivs-77/calc`
1. После успешного выполнения команды будет создана папка calc с подпапками:
  1. src
  1. sql
  1. test
# Настройка БД
1. Настройка брандмауэра:<br/>
`firewall-cmd --add-service=postgresql --permanent`<br/>
`firewall-cmd --reload`<br/>
