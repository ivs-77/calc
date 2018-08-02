# ПО для сборки проекта
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
# Сборка проекта
1. Выбрать папку, в которую будет загружен проект
1. В выбранной папке выполнить команду:<br/> 
`git clone https://github.com/ivs-77/calc`
1. После успешного выполнения команды будет создана папка calc с подпапками:<br/>
src - исходный код проекта<br/>
sql - скрипты для работы с базой данных проекта<br/>
test - исходный код программы нагрузочного тестирования
1. Для сборки проекта в папке calc/src запустить скрипт build.sh
1. В процессе сборки в папке calc будут созданы подпапки:<br/>
build - вспомогательные файлы, необходимые в процессе сборки<br/>
bin - результат сборки проекта, содержит два файла:<br/> 
`  calc` - исполняемый файл<br/>
`  config.txt` - файл сонфигурации, см. раздел 'Конфигурация проекта'
# Конфигурация проекта
Файл config.txt содержит следующие настройки (с примерами значений):
1. `port=1500` - порт, который будет слушать приложение. Для разрешения порта брандмауэром выполнить:<br/>
`firewall-cmd --zone=public --add-port=1500/tcp --permanent`<br/>
`firewall-cmd --reload`
1. `login_timeout=30` - тайм-аут в секундах, в течении которого приложение ожидает команды login/password. Если пользователь не аутентифицируется за указанное время, сессия будет завершена.
1. `connect=host=127.0.0.1 dbname=calc user=calc password=calc` - строка подключения к БД в формате функции PGconnectdb. См. раздел 'Настройка БД'.
1. `stop_key=stop` - команда остановки приложения. Если в новой сессии без аутентификации ввести эту команду, приложение завершает работу.
1. `result_format=%14.3f` - формат, в котором приложение выдает результат вычислений. Данный параметр должен определять формат числа с плавающей точкой для фунции sprintf.
# Настройка БД
1. Установить PostgreSQL<br/>
`yum install postgresql-server postgresql-contrib`
1. Выполнить первоначальную настройку кластера PostgreSQL<br/>
`postgresql-setup initdb`
1. Настроить брандмауэр, чтобы он пропускал трафик PostgreSQL:<br/>
`firewall-cmd --add-service=postgresql --permanent`<br/>
`firewall-cmd --reload`<br/>
1. Войти в систему от имени учетной записи postgres<br/>
`sudo -i -u postgres`
1. В файле `/var/lib/pgsql/data/postgresql.conf` задать параметр `listen_addresses` - какие адреса прослушивать. Например, таким образом - прослушивать все адреса:<br/>
`listen_addresses='*'`
1. В файле `/var/lib/pgsql/data/pg_hba.conf` указать способ подключения для различных адресов/подсетей. Например, для подсети 192.168.1.0/24 и для локального адреса задается способ - ввод логина и пароля таким образом:<br/>
`host    all             all             127.0.0.1/32            md5`<br/>
`host    all             all             192.168.1.0/24          md5`<br/>
`host    all             all             ::1/128                 md5`
1. Запустить PostgreSQL<br/>
`pg_ctl start`
1. Создать пользователя для проекта calc. Например, это будет пользователь calc (в процессе создания скрипт запросит пароль для нового пользователя):<br/>
`createuser -l -P calc`
1. Создать базу данных для пользователя calc. Для простоты, пусть имя БД совпадает с именем созданного пользователя - тогда она автоматически становится его БД по умолчанию:<br/>
`createdb calc`
1. Вернуться в сессию, в которой начали устанавливать PostgreSQL. Войти в БД calc:<br/>
`psql -U calc -h 127.0.0.1`
1. Выполнить скрипт (он также доступен в папке проекта `calc/sql/create_tables.sql`):<br/>
`create table accounts(id integer not null, name varchar(255) not null, pwd varchar(255) not null, amount integer not null);`<br/>
`alter table accounts add primary key (id);`<br/>
`alter table accounts add constraint accounts_name_uk unique (name);`<br/>
`create table account_log(id serial, account_id integer not null, dt timestamp not null default current_timestamp, expr text not null, res varchar(1024) not null);`<br/>
`alter table account_log add primary key (id);`<br/>
`alter table account_log add constraint accounts_fk foreign key (account_id) references accounts (id);`<br/>
1. Для примера можно добавить учетные записи в БД (`calc/sql/insert_sample_data.sql`):<br/>
`insert into accounts (id, name, pwd, amount) values (1, 'john', 'johnpwd', 999);`<br/>
`insert into accounts (id, name, pwd, amount) values (2, 'jack', 'jackpwd', 999);`
1. Указать параметры подключения к созданной БД в файле config.txt собранного сервиса calc (см. раздел 'Конфигурация проекта').
# Запуск проекта
1. После выполнения сборки, конфирурации и настройки БД запустить исполняемый файл calc в папке calc/bin.
1. В случае успешного запуска в папке calc/bin будет создан файл `inf_<дата запуска>_<время запуска>.txt`. В файле будет строка:<br/>
`Listener started. Accepting connections`
