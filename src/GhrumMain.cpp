/*
 * Copyright (c) 2013 Ghrum Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <GhrumEngineServer.hpp>
#include <GhrumAPI.hpp>
#include <boost/program_options.hpp>

/**
 * Initialize and execute the engine.
 *
 * @param mode the mode of the engine
 */
void run(std::string & mode) {
    // Initialize and populate the engine class and
    // descriptor, also the global singleton of it.
    std::unique_ptr<Ghrum::GhrumEngine> engine;
    if (mode.compare("server") == 0) {
        engine = std::unique_ptr<Ghrum::GhrumEngine>(
                     new Ghrum::GhrumEngineServer());
    } else {
        BOOST_LOG_TRIVIAL(error)
                << "Client mode is not supported yet.";
    }
    Ghrum::GhrumAPI::getInstance().setInstance(engine.get());

    // Initialize every engine's component.
    BOOST_LOG_TRIVIAL(info) << "[*] Initializing....";
    engine->initialize();

    // Run into the scheduler's main loop.
    static_cast<Ghrum::Scheduler &>(engine->getScheduler()).runMainThread();

    // Dispose every engine's component allocated.
    BOOST_LOG_TRIVIAL(info) << "[*] Exiting....";
    engine->dispose();
}

/**
 * Entry of the application platform.
 *
 * @param argc number of parameters
 * @param argv parameters
 * @return application exit code
 */
int main(int argc, char * argv[]) {
    boost::program_options::options_description description("Arguments");
    boost::program_options::variables_map vm;

    description.add_options()
    ("help,h", "Show help message.")
    ("mode,m", boost::program_options::value<std::string>(), "Sets the mode of the engine.");

    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, description), vm);
        boost::program_options::notify(vm);

        if ( vm.count("mode") ) {
            std::string mode
                = vm["mode"].as<std::string>();
            run(mode);
        } else {
            std::cout << description << std::endl;
        }
    } catch ( std::exception & ex ) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
    }
    return 0;
}