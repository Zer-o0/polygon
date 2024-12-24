class Solution {
public:
    double mincostToHireWorkers(vector<int>& quality, vector<int>& wage, int groupSize) {
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> minRateHeap;
        priority_queue<int> maxQualityHeap;
        for (int i = 0; i < quality.size(); i++)
            minRateHeap.push({1.0 * wage[i] / quality[i], quality[i]});
        
        double minCost = DBL_MAX;
        long long totalQuality = 0;
        while (!minRateHeap.empty()) {
            auto currentWorker = minRateHeap.top();
            minRateHeap.pop();
            totalQuality += currentWorker.second;
            maxQualityHeap.push(currentWorker.second);
            
            if (maxQualityHeap.size() > groupSize) {
                totalQuality -= maxQualityHeap.top();
                maxQualityHeap.pop();
            }
            
            if (maxQualityHeap.size() == groupSize)
                minCost = min(minCost, totalQuality * currentWorker.first);
        }
        return minCost;
    }
};
