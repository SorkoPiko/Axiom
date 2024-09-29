#ifndef TRAINMANAGER_HPP
#define TRAINMANAGER_HPP

class TrainManager {
    std::unordered_map<int, std::string> m_instances;

    void addInstance(int p1, int p2);

public:
    void assignTask(const std::vector<bool>& instructions, std::function<void(bool, size_t)> callback);

    static TrainManager* create() {
        return new TrainManager();
    }
};

#endif