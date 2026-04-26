

#ifndef main_h
#define main_h
#include <iostream>
#include <random>
#include <cmath>
#include <ctime>
#define SIGN(x) ((x) >= 0 ? 1 : -1)


class Random_Generator{
public:
    virtual ~Random_Generator() {};
    virtual double generate() = 0;
    virtual void get_parameters(double* parameters, int* count) const = 0;
    virtual double get_expected_mean() = 0;
    virtual double get_expected_variance() = 0;
};

class Error{
    const char* text;
public:
    ~Error(){}
    Error(const char* txt): text(txt){}
    const char* ermessage() const{
        return text;
    };
};


class Uniform_Distribution_Generator: public Random_Generator{
    double a;
    double b;
public:
    Uniform_Distribution_Generator(double a_p = 0.0, double b_p = 1.0): a(a_p), b(b_p) {
        if (b < a) throw Error("b must be bigger than a");
    }
    double generate(){
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_real_distribution<> range(a, b);
        return range(gen);
    }
    
    double get_expected_mean(){
        return (a + b) / 2;
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = a;
            parameters[1] = b;
            *count = 2;
        }
    };
    
    double get_expected_variance(){
        return 1.0/12.0 * (b - a) * (b - a);
    }
};

class Cauchy_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double scale;
    double location;
public:
    Cauchy_Distribution_Generator(double sc = 1.0, double loc = 0.0): UniGen(), scale(sc), location(loc){if (scale <= 0) throw Error("Scale must be positive");}
    double generate(){
        return location + scale * tan(M_PI * (UniGen.generate() - 0.5));
        
    }
    double get_expected_mean() {
        throw Error("Cauchy Distribution has no expected mean");
    }
    double get_expected_variance(){
        throw Error("Cauchy Distribution has no expected variance");
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = scale;
            parameters[1] = location;
            *count = 2;
        }
    }
};

class Exponential_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double lambda;
public:
    Exponential_Distribution_Generator(double lamb = 1.0): UniGen(), lambda(lamb){
        if (lambda < 0) throw Error("Lambda parameter must be positive");
    }
    double generate(){
        return -log(1 - UniGen.generate()) / lambda;
    }
    double get_expected_mean(){
        return 1.0 / lambda;
    }
    double get_expected_variance(){
        return 1.0 / lambda / lambda;
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = lambda;
            *count = 1;
        }
    }
};
class Laplace_Distribution_Generator: public Random_Generator{
    double mu;
    double b;
    Uniform_Distribution_Generator UniGen;
public:
    Laplace_Distribution_Generator(double m = 0.0, double scale = 1.0): UniGen(), mu(m), b(scale){
        if (b <= 0) throw Error("Scale parameter must be positive");
    }
    double generate(){
        double u = UniGen.generate();
        return mu + b * SIGN(u - 0.5) * log(1 - 2 * abs(u - 0.5));
    }
    double get_expected_mean(){
        return mu;
    }
    double get_expected_variance(){
        return 2 * b * b;
    }
    
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = mu;
            parameters[1] = b;
            *count = 2;
        }
    }
};


class Normal_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double mean;
    double standard_deviation;
public:
    Normal_Distribution_Generator(double mean = 0.0, double stdev = 1.0): UniGen(-1.0, 1.0), mean(mean), standard_deviation(stdev){if (stdev <= 0) throw Error("Standart deviation must be positive");}
    double generate(){
        double u = UniGen.generate();
        double v = UniGen.generate();
        double s = u * u + v * v;
        if (s > 1) return generate();
        return mean + standard_deviation * u * sqrt(-2.0 * log(s) / s);
    }
    double get_expected_mean(){
        return mean;
    }
    double get_expected_variance(){
        return standard_deviation * standard_deviation;
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = mean;
            parameters[1] = standard_deviation;
            *count = 2;
        }
    }
};

class Beta_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double alpha;
    double beta;
public:
    Beta_Distribution_Generator(double a = 1.0, double b = 0.5): UniGen(), alpha(a), beta(b){
        if (alpha <= 0 || beta <=0 ) throw Error("Alpha and Beta parameters must be positive");}
    double generate(){
        double u = UniGen.generate();
        double v = UniGen.generate();
        double ksi = pow(u, 1 / alpha);
        double eta = pow(v, 1 / beta);
        while(ksi + eta > 1){return generate();}//34543160
        return ksi / (ksi + eta);
    }
    double get_expected_mean(){
        return alpha / (alpha + beta);
    }
    double get_expected_variance(){
        return (alpha * beta) / ((alpha + beta) * (alpha + beta) * (alpha + beta + 1));
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = alpha;
            parameters[1] = beta;
            *count = 2;
        }
    }
    
};

class Gamma_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    Normal_Distribution_Generator NormGen;
    double alpha;
    double theta;
public:
    Gamma_Distribution_Generator(double a = 1.0, double t = 1.0): UniGen(), NormGen(), alpha(a), theta(t){if (alpha <= 0) throw Error("Alpha parameter must be positive");
        if (theta <= 0) throw Error("theta parameter must be positive");
    }
    double generate(){
        if (alpha < 1){
            double b = (exp(1.0) + alpha) / exp(1.0);
            double u = UniGen.generate();
            double P = b * u;
            if (P > 1){
                double y = - log((b - P) / alpha);
                double v = UniGen.generate();
                if (v <= pow(y, alpha - 1.0)){
                    return theta * y;
                }
                else{
                    return generate();
                }
            }
            else{
                double y = pow(P, 1/ alpha);
                double v = UniGen.generate();
                if (v < exp(-y)){
                    return theta * y;
                }
                else{
                    return generate();
                }
            }
        }
        else if (alpha == 1){
            double N = NormGen.generate();
            return N;
        }
        else {
            double d = alpha - 1 / 3;
            double x = NormGen.generate();
            double v = (1 + x / sqrt(9 * d)) * (1 + x / sqrt(9 * d)) * (1 + x / sqrt(9 * d));
            double u = UniGen.generate();
            if (log(u) >= 0.5 * x * x + d - d * v + d * log(v)){
                return generate();
            }
            return d * v * theta;
        }
    }
    double get_expected_mean(){
        return alpha * theta;
    }
    double get_expected_variance(){
        return alpha * theta * theta;
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = alpha;
            parameters[1] = theta;
            *count = 2;
        }
    }
};
class Chi_Squared_Distribution_Generator: public Random_Generator{
    int k;
    Gamma_Distribution_Generator GamGen;
public:
    Chi_Squared_Distribution_Generator(int n = 1): GamGen(k / 2.0, 2.0), k(n) {
        if (k < 0) throw Error("K parameter must be non-negative");
    }
    double generate(){
        return GamGen.generate();
    }
    double get_expected_mean(){
        return k;
    }
    double get_expected_variance(){
        return 2 * k;
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = double(k);
            *count = 1;
        }
    }
};

class Maxwell_Distribution_Generator: public Random_Generator{
    Normal_Distribution_Generator NormGen;
    double a;
public:
    Maxwell_Distribution_Generator(double a_p = 1.0): a(a_p), NormGen(0.0, a_p){
        if (a <= 0) throw Error("a parameter must be positive");
    }
    double generate(){
        double x = NormGen.generate();
        double y = NormGen.generate();
        double z = NormGen.generate();
        return sqrt(x * x + y * y + z * z);
    }
    double get_expected_mean(){
        return 2 * a * sqrt(2.0 / M_PI);
    }
    double get_expected_variance(){
        return a * a * (3.0 - 8.0 / M_PI);
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = a;
            *count = 1;
        }
    }
};

class Bernoulli_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double p;
public:
    Bernoulli_Distribution_Generator(double prob): UniGen(), p(prob){
        if (p < 0 || p > 1) throw Error("P must be in [0, 1] range");
    }
    double generate(){
        double u = UniGen.generate();
        return (u <= p) ? 1.0 : 0.0;
    }
    double get_expected_mean(){
        return p;
    }
    double get_expected_variance(){
            return p * (1 - p);
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = p;
            *count = 1;
        }
    }
};

class Binomial_Distribution_Generator: public Random_Generator{
    Bernoulli_Distribution_Generator BernGen;
    int n;
public:
    Binomial_Distribution_Generator(int trials = 100, double prob = 0.5): BernGen(prob), n(trials){
        if (n<= 0) throw Error("Number of trials must be positive");
    }
    
    double generate(){
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += static_cast<int>(BernGen.generate());
        }
        return static_cast<double>(sum);
    }
    double get_expected_mean(){
        return n * BernGen.get_expected_mean();
    }
    
    double get_expected_variance(){
        return n * BernGen.get_expected_variance();
    }
    void get_parameters(double* parameters, int* count) const{
        if (count && parameters) {
            parameters[0] = static_cast<double>(n);
            *count = 1;
        }
    }
    
};


class Poisson_Distribution_Generator: public Random_Generator{
    Uniform_Distribution_Generator UniGen;
    double lambda;
    double L;
public:
    Poisson_Distribution_Generator(double l = 1.0) : UniGen(), lambda(l) {
            if (lambda <= 0) throw Error("Lambda parameter must be positive");
            L = exp(-lambda);
        }
        
        double generate() {
            int k = 0;
            double p = 1.0;
            
            while (p > L){
                k++;
                double u = UniGen.generate();
                p *= u;
            }
            
            return static_cast<double>(k - 1);
        }
        
        double get_expected_mean() {
            return lambda;
        }
        
        double get_expected_variance() {
            return lambda;
        }
        
        void get_parameters(double* parameters, int* count) const {
            if (count && parameters) {
                parameters[0] = lambda;
                *count = 1;
            }
        }
};


class Empirical_Evaluator{
private:
    double sum;
    double squares_sum;
    int count;
public:
    Empirical_Evaluator(): sum(0), squares_sum(0), count(0){}
    void add_value(double x){
        sum += x;
        squares_sum += x * x;
        count++;
    }
    
    double get_mean() const{
        if (count > 0) return sum / count;
        else return 0.0;
    }
    double get_variance() const{
        if (count <= 1) return 0;
        double m = get_mean();
        return squares_sum / count - m * m;
    }
    double get_standart_deviation() const{
        return sqrt(get_variance());
    };
    void reset(){
        sum = 0;
        squares_sum = 0;
        count = 0;
    }
    Empirical_Evaluator calculate_moments(Random_Generator& RandGen, int sample_size){
        Empirical_Evaluator moments;
        for (int i = 0; i < sample_size; i++){
            moments.add_value(RandGen.generate());
        }
        return moments;
    }
};


class Sampling{
public:
    enum Distribution{
        Uniform,
        Exponential,
        Normal,
        Cauchy,
        Laplace,
        Chi_Squared,
        Gamma,
        Beta,
        Maxwell,
        Binomial,
        Bernoulli,
        Poisson,
    };
    Random_Generator* create(Distribution type, double par1 = 0.0, double par2 = 0.0){
        switch (type) {
            case Uniform:
                return new Uniform_Distribution_Generator();
            case Exponential:
                return new Exponential_Distribution_Generator(par1);
            case Normal:
                return new Normal_Distribution_Generator(par1, par2);
            case Cauchy:
                return new Cauchy_Distribution_Generator(par1, par2);
            case Laplace:
                return new Laplace_Distribution_Generator(par1, par2);
            case Chi_Squared:
                return new Chi_Squared_Distribution_Generator((int)par1);
            case Gamma:
                return new Gamma_Distribution_Generator(par1, par2);
            case Beta:
                return new Beta_Distribution_Generator(par1, par2);
            case Maxwell:
                return new Maxwell_Distribution_Generator(par1);
            case Poisson:
                return new Poisson_Distribution_Generator(par1);
            case Binomial:
                return new Binomial_Distribution_Generator(par1);
            case Bernoulli:
                return new Bernoulli_Distribution_Generator(par1);
            default:
                throw Error("Unknown continuous distribution type");
        }
    }
};


class Statistical_Test{
public:
    void evaluate_moments(Random_Generator& Gen, int sample_size) {
        Empirical_Evaluator edf;
        Empirical_Evaluator moments = edf.calculate_moments(Gen, sample_size);
        try{
            
            std::cout << "\nEmpirical moments (n=" << sample_size << "):" << std::endl;
            std::cout << "  Sample mean: " << moments.get_mean() << std::endl;
            std::cout << "  Sample variance: " << moments.get_variance() << std::endl;
            std::cout << "  Sample std deviation: " << moments.get_standart_deviation() << std::endl;
            
            
            double expectedMean = Gen.get_expected_mean();
            double expectedVar = Gen.get_expected_variance();
            
            std::cout << "\nTheoretical moments:" << std::endl;
            std::cout << "  Expected mean: " << expectedMean << std::endl;
            std::cout << "  Expected variance: " << expectedVar << std::endl;
            
            if (expectedMean != 0) {
                double meanError = fabs(moments.get_mean() - expectedMean) / expectedMean * 100;
                std::cout << "  Mean relative error: " << meanError << "%" << std::endl;
            }
            
            if (expectedVar != 0) {
                double varError = fabs(moments.get_variance() - expectedVar) / expectedVar * 100;
                std::cout << "  Variance relative error: " << varError << "%" << std::endl;
            }
        }
        catch (const Error& e) {
            std::cout << "An error occurred: " << e.ermessage() << std::endl;
        }
    }
};



class Presenter{
private:
    Sampling sample;
        
    void test(Random_Generator* RGen, int sample = 10000){
        std::cout << "First 10 generated values:" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << RGen->generate() << " ";
        }
        std::cout << std::endl;
        
        Statistical_Test t;
        std::cout << "\ntests.evaluateAndPrintMoments: " << std::endl;
        t.evaluate_moments(*RGen, sample);
        std::cout << std::endl;
            
    }
        
    void run_all(){
        std::cout << "\nTESTING ALL DISTRIBUTIONS" << std::endl;
        Sampling s;
        
        std::cout << "\n1. Binomial distribution (n=10, p=0.3):" << std::endl;
        Random_Generator* BinomGen =  s.create(Sampling::Binomial, 10, 0.3);
        test(BinomGen, 10000);
        delete BinomGen;
        
        std::cout << "\n2. Poisson distribution (lambda=1):" << std::endl;
        Random_Generator* PoissonGen = s.create(Sampling::Poisson, 1);
        test(PoissonGen, 10000);
        delete PoissonGen;
        
        std::cout << "\n3. Bernoulli distribution (p=0.7):" << std::endl;
        Random_Generator* BernoulliGen = s.create(Sampling::Bernoulli, 0.7);
        test(BernoulliGen, 10000);
        delete BernoulliGen;
        
        std::cout << "\n4. Uniform distribution:" << std::endl;
        Random_Generator* UniformGen = s.create(Sampling::Uniform);
        test(UniformGen, 10000);
        delete UniformGen;
        
        std::cout << "\n5. Exponential distribution (lambda=1.5):" << std::endl;
        Random_Generator* ExpGen = s.create(Sampling::Exponential, 1.5);
        test(ExpGen, 10000);
        delete ExpGen;
        
        std::cout << "\n6. Standard normal distribution:" << std::endl;
        Random_Generator* NormGen = s.create(Sampling::Normal, 0, 1.0);
        test(NormGen, 10000);
        delete NormGen;
        
        std::cout << "\n7. Standard Cauchy distribution:" << std::endl;
        Random_Generator* CauchyGen = s.create(Sampling::Cauchy, 1.0);
        test(CauchyGen, 10000);
        delete CauchyGen;
        
        std::cout << "\n8. Laplace distribution:" << std::endl;
        Random_Generator* LaplaceGen = s.create(Sampling::Laplace, 0.0, 1.0);
        test(LaplaceGen, 10000);
        delete LaplaceGen;
        
        std::cout << "\n9. Chi-squared distribution (k = 3):" << std::endl;
        Random_Generator* ChiGen = s.create(Sampling::Chi_Squared, 1);
        test(ChiGen, 10000);
        delete ChiGen;
        
        std::cout << "\n10. Gamma distribution (1, 1):" << std::endl;
        Random_Generator* GammaGen = s.create(Sampling::Gamma, 1.0, 1.0);
        test(GammaGen, 10000);
        delete GammaGen;
        
        std::cout << "\n11. Beta distribution (1, 0.5):" << std::endl;
        Random_Generator* BetaGen = s.create(Sampling::Beta, 1.0, 1.0);
        test(BetaGen, 10000);
        delete BetaGen;
        
        std::cout << "\n12. Maxwell distribution (2):" << std::endl;
        Random_Generator* MaxGen = s.create(Sampling::Maxwell, 2.0);
        test(MaxGen, 10000);
        delete MaxGen;
    }
        
    Random_Generator* create_Exponential(){
        std::cout << "\nExponential Distribution" << std::endl;
        std::cout << "Enter lambda parameter > 0: ";
        double lambda;
        std::cin >> lambda;
            
        if (lambda <= 0) {
            throw Error("Lambda parameter must be positive");
        }
            
        return new Exponential_Distribution_Generator(lambda);
    };
        
    Random_Generator* create_Normal(){
        std::cout << "\nNormal Distribution" << std::endl;
        std::cout << "Enter mean: ";
        double mean;
        std::cin >> mean;
            
        std::cout << "Enter standard deviation > 0: ";
        double stddev;
        std::cin >> stddev;
            
        if (stddev <= 0) {
            throw Error("Standard deviation must be positive");
        }
            
        return new Normal_Distribution_Generator(mean, stddev);
    }
        
    Random_Generator* create_Cauchy(){
        std::cout << "\nCauchy Distribution" << std::endl;
        std::cout << "Enter location parameter: ";
        double location;
        std::cin >> location;
            
        std::cout << "Enter scale parameter > 0: ";
        double scale;
        std::cin >> scale;
            
        if (scale <= 0) {
            throw Error("Scale must be positive");
        }
            
        return new Cauchy_Distribution_Generator(location, scale);
    }
        
    Random_Generator* create_Laplace(){
        std::cout << "\nLaplace Distribution" << std::endl;
        std::cout << "Enter location parameter: ";
        double location;
        std::cin >> location;
            
        std::cout << "Enter scale parameter > 0: ";
        double scale;
        std::cin >> scale;
            
        if (scale <= 0) {
            throw Error("Scale must be positive");
        }
            
        return new Laplace_Distribution_Generator(location, scale);
    }
        
    Random_Generator* create_Binomial(){
        std::cout << "\nBinomial Distribution" << std::endl;
        std::cout << "Enter n (integer > 0): ";
        int n;
        std::cin >> n;
        if (n <= 0){
            throw Error("Number of trials must be positive");
        }
        double p;
        std::cout << "Enter p in range [0, 1]: ";
        std::cin >> p;
        if (p > 1 || p < 0){
            throw Error("Probability p must be in range [0, 1]");
        }
        return new Binomial_Distribution_Generator(n, p);
    }
        
        
    Random_Generator* create_Poisson(){
        std::cout << "\nPoisson Distribution" << std::endl;
        std::cout << "Enter lambda > 0: ";
        double lambda;
        std::cin >> lambda;
        if (lambda <= 0){
            throw Error("Lambda parameter must be positive");
        }
        return new Poisson_Distribution_Generator(lambda);
    }
        
    Random_Generator* create_Bernoulli(){
        std::cout << "\nBernoulli Distribution" << std::endl;
        std::cout << "Enter p in range [0, 1]: ";
        double p;
        std::cin >> p;
        if (p < 0 || p > 1){
            throw Error("Probability p must be in range [0, 1]");
        }
        return new Bernoulli_Distribution_Generator(p);
    }
        
    Random_Generator* create_Uniform(){
        std::cout << "\nUniform Distribution" << std::endl;
        std::cout << "Enter a: " << std::endl;
        double a;
        std::cin >> a;
        std::cout << "Enter b: " << std::endl;
        double b;
        std::cin >> b;
        if (b <= a) {
            throw Error("Upper bound b must be greater than lower bound a");
        }
        return new Uniform_Distribution_Generator(a, b);
    }
        
    Random_Generator* create_Maxwell(){
        std::cout << "\nMaxwell Distribution" << std::endl;
        
        std::cout << "Enter a > 0: ";
        double a;
        std::cin >> a;
        if (a <= 0){
            throw Error("Scale parameter a must be positive");
        }
        return new Maxwell_Distribution_Generator(a);
    }
        
    Random_Generator* create_Gamma(){
        std::cout << "\nGamma Distribution" << std::endl;
        double alpha;
        double theta;
        std::cout << "Enter alpha > 0: " << std::endl;
        std::cin >> alpha;
        if (alpha <= 0){
            throw Error("Shape parameter alpha must be positive");
        }
        std::cout << "Enter theta > 0: " << std::endl;
        std::cin >> theta;
        if (theta <= 0){
            throw Error("Scale parameter theta must be positive");
        }
        return new Gamma_Distribution_Generator(alpha, theta);
    }
        
    Random_Generator* create_Beta(){
        std::cout << "\nBeta Distribution" << std::endl;
        double alpha;
        double beta;
        std::cout << "Enter alpha > 0" << std::endl;
        std::cin >> alpha;
        if (alpha <= 0){
            throw Error("Alpha parameter must be positive");
        }
        std::cout << "Enter beta > 0" << std::endl;
        std::cin >> beta;
        if(beta <= 0){
            throw Error("Beta parameter must be positive");
        }
        return new Beta_Distribution_Generator(alpha, beta);
    }
        
    Random_Generator* create_Chi_Squared(){
        std::cout << "\nChi-squared Distribution" << std::endl;
        int k;
        std::cout << "Enter k >= 0" << std::endl;
        std::cin >> k;
        if (k < 0){
            throw Error("Degrees of freedom k must be non-negative");
        }
        return new Chi_Squared_Distribution_Generator(k);
    }
    public:
        enum Distribution{
            Binomial = 1,
            Poisson,
            Bernoulli,
            Uniform,
            Exponential,
            Normal,
            Cauchy,
            Laplace,
            Chi_Squared,
            Gamma,
            Beta,
            Maxwell,
            All,
            End = 0,
        };
        void print_header(){
            std::cout << "RANDOM VARIABLE GENERATOR" << std::endl;
        }
        void print_menu(){
            std::cout << "\nChoose distribution type:" << std::endl;
            std::cout << "1. Binomial (n, p)" << std::endl;
            std::cout << "2. Poisson (lambda)" << std::endl;
            std::cout << "3. Bernoulli (p)" << std::endl;
            std::cout << "4. Uniform ()" << std::endl;
            std::cout << "5. Exponential (lambda)" << std::endl;
            std::cout << "6. Normal (mean, stddev)" << std::endl;
            std::cout << "7. Cauchy (location, scale)" << std::endl;
            std::cout << "8. Laplace (location, scale)" << std::endl;
            std::cout << "9. Chi-squared (k)" << std::endl;
            std::cout << "10. Gamma (alpha, theta)" << std::endl;
            std::cout << "11. Beta (alpha, beta)" << std::endl;
            std::cout << "12. Maxwell (a)" << std::endl;
            std::cout << "13. Test all distributions" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "Your choice: ";
        }
    void run(){
        print_header();
        print_menu();
        bool end_flag = false;
        int choice;
        std::cin >> choice;
        if (choice == End) {
            std::cout << "\nGoodbye!" << std::endl;
            return;
        }
        while (end_flag == false){
            try{
                Random_Generator* generator = nullptr;
                
                if (choice == Binomial) {
                    generator = create_Binomial();
                    end_flag = 1;
                }
                else if (choice == Poisson) {
                    generator = create_Poisson();
                    end_flag = 1;
                }
                else if (choice == Bernoulli) {
                    generator = create_Bernoulli();
                    end_flag = 1;
                }
                else if (choice == Uniform) {
                    generator = create_Uniform();
                    end_flag = 1;
                }
                else if (choice == Exponential) {
                    generator = create_Exponential();
                    end_flag = 1;
                }
                else if (choice == Normal) {
                    generator = create_Normal();
                    end_flag = 1;
                }
                else if (choice == Cauchy) {
                    generator = create_Cauchy();
                    end_flag = 1;
                }
                else if (choice == Laplace) {
                    generator = create_Laplace();
                    end_flag = 1;
                }
                else if (choice == Maxwell) {
                    generator = create_Maxwell();
                    end_flag = 1;
                }
                else if (choice == Beta) {
                    generator = create_Beta();
                    end_flag = 1;
                }
                else if (choice == Gamma) {
                    generator = create_Gamma();
                    end_flag = 1;
                }
                else if (choice == All) {
                    run_all();
                    end_flag = 1;
                }
                else {
                    std::cout << "Invalid choice! Please try again." << std::endl;
                    end_flag = 1;
                }
                
                if (generator) {
                    std::cout << "Start Test:" << std::endl;
                    test(generator);
                    delete generator;
                }
                
            } catch (const Error& e) {
                std::cout << "An error occurred: " << e.ermessage() << std::endl;
            }
        }
    }
};


#endif /* main_h */
