#include <type.hpp>
#include <eosio/singleton.hpp>

CONTRACT oracle : public contract
{
public:
   oracle(name receiver, name code, datastream<const char *> ds)
       : contract(receiver, code, ds),
         _globals(_self, _self.value),
         _sender(_self) {}

   ACTION init(name source);
   ACTION update(name submitter, uint64_t pairid);
   ACTION log(st_log data);

   using init_action = action_wrapper<"init"_n, &oracle::init>;
   using update_action = action_wrapper<"update"_n, &oracle::update>;
   using log_action = action_wrapper<"log"_n, &oracle::log>;

private:
   TABLE avgprice
   {
      uint64_t key;

      name submitter;
      string period;

      uint64_t price0_cumulative_last;
      uint64_t price1_cumulative_last;

      double price0_avg_price;
      double price1_avg_price;

      time_point_sec last_update;

      uint64_t primary_key() const { return key; }

      EOSLIB_SERIALIZE(avgprice, (key)(submitter)(period)(price0_cumulative_last)(price1_cumulative_last)(price0_avg_price)(price1_avg_price)(last_update))
   };

   TABLE globals
   {
      name source;

      EOSLIB_SERIALIZE(globals, (source))
   };

   typedef multi_index<"avgprices"_n, avgprice> avgprices;

   typedef eosio::singleton<"globals"_n, globals> globals_index;
   globals_index _globals;

   name _sender;
   uint64_t _pairid;

   void update_all(const market pricedata);
   void update_price(const string period, const market pricedata, const uint64_t key);
};