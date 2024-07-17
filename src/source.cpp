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
std::string bot_id = "1117481558492839987";
std::string bot_channel = "1117854607712981013";
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, bool> active_request;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, bool> active_dialog;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, SleepyDiscord::Snowflake<SleepyDiscord::User>> reference;
std::queue<std::pair<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, std::string>> MessageQueue;
std::vector<std::pair<int, SleepyDiscord::Snowflake<SleepyDiscord::User>>> q;
std::vector<std::tuple<SleepyDiscord::Snowflake<SleepyDiscord::Channel>, SleepyDiscord::Snowflake<SleepyDiscord::Channel>, std::time_t>> couples;
std::unordered_map<SleepyDiscord::Snowflake<SleepyDiscord::User>, SleepyDiscord::Snowflake<SleepyDiscord::Channel>> activeUsers;
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
        createGlobalAppCommand(getID(), "dialog", "Начать диалог с загадочным незнакомцем");
        createGlobalAppCommand(getID(), "stop", "Завершить активный диалог/запрос");
        createGlobalAppCommand(getID(), "help", "Как начать пользоваться приложением?");
        std::thread(&MyClientClass::processMessageQueue, this).detach();
        std::thread(&MyClientClass::checkDialogTimeouts, this).detach();
    }

    void onMessage(SleepyDiscord::Message message) override {
        std::cout << "Message received: " << message.content << std::endl;
        
        if (message.content.empty()) {
            sendMessage(message.channelID, "Пользователь отправил недопустимое сообщение! 🛑");
            return;
        }

        auto it = std::find_if(couples.begin(), couples.end(), [&](const auto& p) {
            return (std::get<0>(p) == message.channelID && message.author.ID != bot_id) ||
                   (std::get<1>(p) == message.channelID && message.author.ID != bot_id);
        });

        if (it != couples.end()) {
            if (std::get<0>(*it) == message.channelID && activeUsers[message.author.ID] != message.channelID) {
                if (!message.attachments.empty()) {
                    MessageQueue.push({std::get<1>(*it), "Пользователь отправил недопустимый файл! 🛑"});
                } else {
                    MessageQueue.push({std::get<1>(*it), message.content});
                }
            } else if (std::get<1>(*it) == message.channelID && activeUsers[message.author.ID] != message.channelID) {
                if (!message.attachments.empty()) {
                    MessageQueue.push({std::get<0>(*it), "Пользователь отправил недопустимый файл! 🛑"});
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
                res.data.content = "Извините, у вас уже есть активный собеседник. 🧐";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);
                return;
            }

            bool hasActivePartner = false;
            if (active_request[User_ID]) hasActivePartner = true;

            if (hasActivePartner) {
                SleepyDiscord::Interaction::Response<> res;
                res.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                res.data.content = "Извините, у вас уже есть активный запрос. Вы всегда можете его отменить, используя команду /stop. 🛑";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);
                return;
            }

            count_value++;
            active_request[User_ID] = true;

            SleepyDiscord::Interaction::Response<> response;
            response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
            response.data.content = "Поиск собеседника начат! Вы можете отменить его в любой момент, воспользовавшись командой /stop 🤝";
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
                        sendMessage(directMessageChannel1.ID, "Собеседник найден 👋! Вы можете прекратить общение в любой момент, используя команду /stop. Беседа автоматически закроется через час после начала. 💖");
                        sendMessage(directMessageChannel2.ID, "Собеседник найден 👋! Вы можете прекратить общение в любой момент, используя команду /stop. Беседа автоматически закроется через час после начала. 💖");

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
                    response.data.content = "Диалог успешно завершён. Чтобы начать новый разговор, используйте команду /dialog. 😶‍🌫️";

                    sendMessage(std::get<0>(*it) == interaction.channelID ? std::get<1>(*it) : std::get<0>(*it), "Ваш собеседник завершил разговор! 💔");
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
                res.data.content = "Поиск диалога успешно отменён, чтобы повторно начать новый диалог, используйте команду /dialog. 😶‍🌫️";
                res.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, res);

                return;
            } 

            if (!found) {
                SleepyDiscord::Interaction::Response<> response;
                response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
                response.data.content = "Извините, у вас нету активного запроса, чтобы начать новый диалог, используйте команду /dialog. 🛑";
                response.data.flags = SleepyDiscord::InteractionCallback::Message::Flags::Ephemeral;
                createInteractionResponse(interaction.ID, interaction.token, response);
            }
            return;
        } else if(interaction.data.name == "help"){
            SleepyDiscord::Interaction::Response<> response;
            response.type = SleepyDiscord::InteractionCallbackType::ChannelMessageWithSource;
            response.data.content = "Спасибо Вам что используете наше приложение! 💖\nДля того чтоб начать пользоваться нашим приложением зайдите в любой для Вас удобный текстовый канал до которого бот имеет доступ, либо зайдите к нему в Личные Сообщения. После чего напишите команду /dialog и следуйте указаниям! 🔥";
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
                if (std::difftime(now, std::get<2>(*it)) > 30) {
                    sendMessage(std::get<0>(*it), "Диалог завершён автоматически через один час. 💔");
                    sendMessage(std::get<1>(*it), "Диалог завершён автоматически через один час. 💔");
                    activeUsers.erase(getUserByChannelID(std::get<0>(*it)));
                    activeUsers.erase(getUserByChannelID(std::get<1>(*it)));
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
    std::string token = "YOUR_BOT_TOKEN";
    MyClientClass client(token, SleepyDiscord::USER_CONTROLED_THREADS);
    client.setIntents(SleepyDiscord::Intent::SERVER_MESSAGES | SleepyDiscord::Intent::DIRECT_MESSAGES);
    client.run();
}
