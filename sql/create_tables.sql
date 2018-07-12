create table accounts(id integer not null, name varchar(255) not null, pwd varchar(255) not null, amount integer not null);
alter table accounts add primary key (id);
alter table accounts add constraint accounts_name_uk unique (name);
create table account_log(id serial, account_id integer not null, dt timestamp not null default current_timestamp, expr text not null, res varchar(1024) not null);
alter table account_log add primary key (id);
alter table account_log add constraint accounts_fk foreign key (account_id) references accounts (id);
