#include <iostream>
#include <queue>
#include <algorithm>
#include <string_view>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <ctime>
#include "sleepy_discord/sleepy_discord.h"

struct adj {
    SleepyDiscord::Snowflake<SleepyDiscord::Channel> v1;
    SleepyDiscord::Snowflake<SleepyDiscord::Server> v2;
    SleepyDiscord::Snowflake<SleepyDiscord::User> v3;
};
int count_value = 0;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, bool> active_request;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, bool> active_dialog;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, SleepyDiscord::Snowflake<SleepyDiscord::User>> reference;
std::queue<std::pair<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, std::string>> MessageQueue;
std::vector<std::pair<int, SleepyDiscord::Snowflake<SleepyDiscord::User>>> q;
std::vector<std::tuple<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, SleepyDiscord::Snowflake<SleepyDiscord::Channel>, std::time_t>> couples;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, SleepyDiscord::Snowflake<SleepyDiscord::Channel>> activeUsers;

std::string bot_ID = "YOUR_BOT_ID";

bool containsEmoji(const std::string& s) {
    try {
        std::string test = s;
    } catch (...) {
        return true;
    }
    return false;
}

class MyClientClass : public SleepyDiscord::DiscordClient {
public:
    using SleepyDiscord::DiscordClient::DiscordClient;

    void onReady(SleepyDiscord::Ready readyData) override {
        std::cout << "Bot is ready!" << std::endl;
        createGlobalAppCommand(getID(), "dialog", "Start a dialog with a mysterious stranger");
        createGlobalAppCommand(getID(), "stop", "End the active dialog/request");
        createGlobalAppCommand(getID(), "help", "How to start using the app?");
        std::thread(&MyClientClass::processMessageQueue, this).detach();
        std::thread(&MyClientClass::checkDialogTimeouts, this).detach();
    }

    void onMessage(SleepyDiscord::Message message) override {
    std::cout << "Message received: " << message.content << std::endl;
    if(message.author.ID == bot_ID) return;

    if (message.content.empty() && message.serverID.empty()) {
        sendMessage(message.channelID, "User sent an invalid message! 🛑");
        return;
    }

    auto it = std::find_if(couples.begin(), couples.end(), [&](const auto& p) {
        return (std::get<0>(p) == message.channelID) || (std::get<1>(p) == message.channelID);
    });

    
    if (it != couples.end()) {
        if (std::get<0>(*it) == message.channelID && activeUsers[message.author.ID] != message.channelID) {
            if (!message.attachments.empty()) {
                MessageQueue.push({std::get<1>(*it), "User sent an invalid file! 🛑"});
            } else {
                MessageQueue.push({std::get<1>(*it), message.content});
            }
        } else if (std::get<1>(*it) == message.channelID && activeUsers[message.author.ID] != message.channelID) {
            if (!message.attachments.empty()) {
                MessageQueue.push({std::get<0>(*it), "User sent an invalid file! 🛑"});
            } else {
                MessageQueue.push({std::get<0>(*it), message.content});
                }
            }
        }
    }
    

    void onInteraction(SleepyDiscord::Interaction interaction) override {
        std::cout << "Interaction received: " << interaction.data.name << std::endl;

        if (interaction.data.name == "dialog") {
            SleepyDiscord::Snowflake<SleepyDiscord::User> User_ID;
            if(!interaction.serverID.empty()) User_ID = interaction.member.user.ID;
            else User_ID = interaction.user.ID;

            if (active_dialog[User_ID]) {
                SleepyDiscord::Interaction::Response<> res;
                res.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                res.data.content = "Sorry, you already have an active partner. 🧐";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);
                return;
            }

            bool hasActivePartner = false;
            if (active_request[User_ID]) hasActivePartner = true;

            if (hasActivePartner) {
                SleepyDiscord::Interaction::Response<> res;
                res.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                res.data.content = "Sorry, you already have an active request. You can always cancel it using the **/stop** command. 🛑";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);
                return;
            }

            count_value++;
            active_request[User_ID] = true;

            SleepyDiscord::Interaction::Response<> response;
            response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
            response.data.content = "Search for a partner has begun! You can cancel it at any time using the **/stop** command. 🤝";
            response.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
            createInteractionResponse(interaction.ID, interaction.token, response);
            q.push_back({count_value, User_ID});

            if (q.size() > 1) {
                bool found = false;
                for (size_t i = 0; i < q.size() - 1 && !found; i++) {
                    for (size_t j = i + 1; j < q.size() && !found; j++) {
                        SleepyDiscord::Channel directMessageChannel1 = createDirectMessageChannel(q[i].second);
                        SleepyDiscord::Channel directMessageChannel2 = createDirectMessageChannel(q[j].second);

                        active_dialog[q[i].second] = true;
                        active_dialog[q[j].second] = true;

                        reference[directMessageChannel1.ID] = q[i].second;
                        reference[directMessageChannel2.ID] = q[j].second;
                        couples.push_back({directMessageChannel1.ID, directMessageChannel2.ID, std::time(nullptr)});
                        sendMessage(directMessageChannel1.ID, "Partner found 👋! You can end the conversation at any time using the **/stop** command. The chat will automatically close an hour after it starts. 💖");
                        sendMessage(directMessageChannel2.ID, "Partner found 👋! You can end the conversation at any time using the **/stop** command. The chat will automatically close an hour after it starts. 💖");

                        q.erase(q.begin() + j);
                        q.erase(q.begin() + i);
                    }
                }
            }
            return;
        }

        else if (interaction.data.name == "stop") {
            SleepyDiscord::Snowflake<SleepyDiscord::User> User_ID;
            if(!interaction.serverID.empty()) User_ID = interaction.member.user.ID;
            else User_ID = interaction.user.ID;

            bool found = false;

            for (auto it = couples.begin(); it != couples.end(); ++it) {
                if (reference[std::get<0>(*it)] == User_ID || reference[std::get<1>(*it)] == User_ID) {
                    found = true;
                    auto userId = User_ID;

                    active_dialog[reference[std::get<0>(*it)]] = false;
                    active_dialog[reference[std::get<1>(*it)]] = false;

                    active_request[reference[std::get<0>(*it)]] = false;
                    active_request[reference[std::get<1>(*it)]] = false;

                    SleepyDiscord::Interaction::Response<> response;
                    response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                    response.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                    response.data.content = "The dialog was successfully completed. To start a new conversation, use the **/dialog** command. 😶‍🌫️";

                    sendMessage(std::get<0>(*it) == interaction.channelID ? std::get<1>(*it) : std::get<0>(*it), "Your partner ended the conversation! 💔");
                    createInteractionResponse(interaction.ID, interaction.token, response);

                    couples.erase(it);
                    return;
                }
            }

            if (active_request[User_ID] == true) {
                found = true;
                for (size_t i = 0; i < q.size(); ++i) {
                    if (q[i].second == User_ID) {
                        q.erase(q.begin() + i);
                        break;
                    }
                }
                active_request[User_ID] = false;

                SleepyDiscord::Interaction::Response<> res;
                res.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                res.data.content = "Search for a dialog successfully canceled. To start a new dialog, use the **/dialog** command. 😶‍🌫️";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);

                return;
            } 

            if (!found) {
                SleepyDiscord::Interaction::Response<> response;
                response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                response.data.content = "Sorry, you don't have an active request. To start a new dialog, use the **/dialog** command. 🛑";
                response.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, response);
            }
            return;
        } else if(interaction.data.name == "help"){
            SleepyDiscord::Interaction::Response<> response;
            response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
            response.data.content = "Thank you for using our application! 💖\nTo start using our application, go to any text channel that the bot has access to, or send it a direct message. Then use the **/dialog** command and follow the instructions! 🔥";
            response.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
            createInteractionResponse(interaction.ID, interaction.token, response);
        }
    }

private:
    void processMessageQueue() {
        while (true) {
            if (!MessageQueue.empty()) {
                auto message = MessageQueue.front();
                sendMessage(message.first, message.second);
                MessageQueue.pop();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    void checkDialogTimeouts() {
        while (true) {
            auto now = std::time(nullptr);
            for (auto it = couples.begin(); it != couples.end();) {
                if (std::difftime(now, std::get<2>(*it)) > 1000) {
                    sendMessage(std::get<0>(*it), "Dialog ended automatically. 💔");
                    sendMessage(std::get<1>(*it), "Dialog ended automatically. 💔");
                    activeUsers.erase(getUserByChannelID(std::get<0>(*it)));
                    active_request[reference[std::get<0>(*it)]] = false;
                    active_dialog[reference[std::get<0>(*it)]] = false;
                    activeUsers.erase(getUserByChannelID(std::get<1>(*it)));
                    active_request[reference[std::get<1>(*it)]] = false;
                    active_dialog[reference[std::get<1>(*it)]] = false;
                    it = couples.erase(it);
                } else {
                    ++it;
                }
            }
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    }

    SleepyDiscord::Snowflake<SleepyDiscord::User> getUserByChannelID(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channelID) {
        for (const auto& userChannel : activeUsers) {
            if (userChannel.second == channelID) {
                return userChannel.first;
            }
        }
        return {};
    }
};

int main() {
    MyClientClass client("YOUR_BOT_TOKEN", SleepyDiscord::USER_CONTROLED_THREADS);
    client.setIntents(SleepyDiscord::Intent::SERVER_MESSAGES | SleepyDiscord::Intent::DIRECT_MESSAGES);
    client.run();
}
