create table accounts(id integer not null, name varchar(255) not null, pwd varchar(255) not null, amount integer not null);
alter table accounts add primary key (id);
alter table accounts add constraint accounts_name_uk unique (name);