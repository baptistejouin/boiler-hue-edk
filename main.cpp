#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <huestream/config/Config.h>
#include <huestream/HueStream.h>
#include "effects/FadeEffect.h"

using namespace huestream;

// Global flag for signal handling
std::atomic<bool> g_shutdownRequested(false);

// Signal handler for Ctrl+C
void signalHandler(int signum) {
    std::cout << "\n\n🛑 Ctrl+C detected! Initiating graceful shutdown..." << std::endl;
    g_shutdownRequested = true;
}

void connectToBridge(std::shared_ptr<HueStream> huestream) {
    std::cout << "\n🔗 Connecting to bridge..." << std::endl;

    huestream->ConnectBridge();

    while (!huestream->IsStreamableBridgeLoaded() && !huestream->IsBridgeStreaming()) {
        auto bridge = huestream->GetLoadedBridge();

        std::cout << "🔍 Bridge status: " << bridge->GetStatusTag() << std::endl;

        if (bridge->GetStatus() == BRIDGE_INVALID_GROUP_SELECTED) {
            std::cout << "📋 Selecting first entertainment group..." << std::endl;
            huestream->SelectGroup(bridge->GetGroups()->at(0));
        } else if (bridge->GetStatus() != BRIDGE_READY && bridge->GetStatus() != BRIDGE_STREAMING) {
            std::cout << "❌ No streamable bridge configured: " << bridge->GetStatusTag() << std::endl;
            std::cout << "Press Enter to retry..." << std::endl;
            std::cin.get();
            huestream->ConnectBridge();
        }
    }

    std::cout << "✅ Bridge connection completed!" << std::endl;
}

int main(int argc, char *argv[]) {
    // Register signal handler for Ctrl+C
    std::signal(SIGINT, signalHandler);

    std::cout << "🌟 Hue Light Effects" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "Choose an effect:" << std::endl;
    std::cout << "1. Fade In/Out" << std::endl;

    std::cout << "Enter your choice (1): ";

    int choice;
    std::cin >> choice;

    if (choice != 1) {
        std::cout << "❌ Invalid choice. Exiting..." << std::endl;
        return 1;
    }

    try {
        // Setup HueStream
        std::cout << "🏠 Setting up HueStream library..." << std::endl;
        auto config = std::make_shared<Config>("LightEffects", "MacBook", PersistenceEncryptionKey("secret_key"));
        auto huestream = std::make_shared<HueStream>(config);

        huestream->RegisterFeedbackCallback([](const FeedbackMessage &message) {
            std::cout << "[" << message.GetId() << "] " << message.GetTag() << std::endl;
            if (message.GetId() == FeedbackMessage::ID_DONE_COMPLETED) {
                std::cout << "✅ Connected to bridge with ip: " << message.GetBridge()->GetIpAddress() << std::endl;
            }
            if (message.GetMessageType() == FeedbackMessage::FEEDBACK_TYPE_USER) {
                std::cout << "👤 " << message.GetUserMessage() << std::endl;
            }
        });

        std::cout << "✅ HueStream library initialized" << std::endl;

        // Connect to bridge
        connectToBridge(huestream);

        // Play selected effect
        if (huestream->IsStreamableBridgeLoaded() || huestream->IsBridgeStreaming()) {
            if (choice == 1) {
                std::cout << "\n🎭 Starting Fade In/Out effect..." << std::endl;
                FadeEffect fadeEffect(huestream);
                fadeEffect.play(g_shutdownRequested);
            }
        } else {
            std::cout << "\n⚠️  No streamable bridge available" << std::endl;
            std::cout << "💡 Configure an entertainment area in the Philips Hue app" << std::endl;
        }

        // Shutdown
        std::cout << "\n🛑 Shutting down HueStream library..." << std::endl;
        huestream->ShutDown();
        std::cout << "✅ Shutdown completed!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
