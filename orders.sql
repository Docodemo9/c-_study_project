create table orders(
	id serial primary key,
	user_id integer,
	item_id integer references assortiment(id)
)


insert into orders(user_id,item_id) values
	(1,1),
	(1,2),
	(2,3),
	(3,2)