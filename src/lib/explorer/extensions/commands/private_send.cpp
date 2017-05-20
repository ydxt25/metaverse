/**
 * Copyright (c) 2016-2017 mvs developers 
 *
 * This file is part of metaverse-explorer.
 *
 * metaverse-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/property_tree/ptree.hpp>      
#include <boost/property_tree/json_parser.hpp>

#include <metaverse/explorer/extensions/commands/private_send.hpp>
#include <metaverse/explorer/extensions/base_helper.hpp>
#include <metaverse/explorer/prop_tree.hpp>

namespace libbitcoin {
namespace explorer {
namespace commands {

namespace pt = boost::property_tree;

/************************ sendwithmsg *************************/

console_result sendwithmsg::invoke (std::ostream& output,
        std::ostream& cerr, bc::blockchain::block_chain_impl& blockchain)
{
	blockchain.is_account_passwd_valid(auth_.name, auth_.auth);

	// receiver
	std::vector<receiver_record> receiver{
		{argument_.address, "", argument_.amount, 0, utxo_attach_type::etp, attachment()}, // "" == etp or "xxx" == asset symbol 
		{argument_.address, "", 0, 0, utxo_attach_type::message, attachment(0, 0, blockchain_message(argument_.message))} // "" == etp or "xxx" == asset symbol 
	};
	auto send_helper = sending_etp(blockchain, std::move(auth_.name), std::move(auth_.auth), 
			"", std::move(receiver), argument_.fee);
	
	send_helper.exec();

	// json output
	auto tx = send_helper.get_transaction();
	pt::write_json(output, prop_tree(tx, true));

	return console_result::okay;
}

/************************ sendwithmsgfrom *************************/

console_result sendwithmsgfrom::invoke (std::ostream& output,
        std::ostream& cerr, bc::blockchain::block_chain_impl& blockchain)
{
	blockchain.is_account_passwd_valid(auth_.name, auth_.auth);
	if(!blockchain.is_valid_address(argument_.from)) 
		throw std::logic_error{"invalid from address!"};
	if(!blockchain.is_valid_address(argument_.to)) 
		throw std::logic_error{"invalid to address!"};
	
	// receiver
	std::vector<receiver_record> receiver{
		{argument_.to, "", argument_.amount, 0, utxo_attach_type::etp, attachment()}, // "" == etp or "xxx" == asset symbol 
		{argument_.to, "", 0, 0, utxo_attach_type::message, attachment(0, 0, blockchain_message(argument_.message))} // "" == etp or "xxx" == asset symbol 
	};
	auto send_helper = sending_etp(blockchain, std::move(auth_.name), std::move(auth_.auth), 
			std::move(argument_.from), std::move(receiver), argument_.fee);
	
	send_helper.exec();

	// json output
	auto tx = send_helper.get_transaction();
	pt::write_json(output, prop_tree(tx, true));

	return console_result::okay;
}


} //commands
} // explorer
} // libbitcoin
