pipeline
{
    agent none
    stages
    {
        stage("Init")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh 'git submodule update --init --recursive'
                        }
                    }
                }
            }
        }
        stage("Build")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh 'cmake . -GNinja -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -Wno-dev'
                            sh 'cmake --build .'
                        }
                    }
                }
            }
        }
        stage("Test")
        {
            parallel
            {
                stage('Linux')
                {
                    agent
                    {
                        label "Linux"
                    }
                    steps
                    {
                        script
                        {
                            sh 'ctest -T test --verbose'
                        }
                        xunit (
                            testTimeMargin: '3000',
                            thresholdMode: 1,
                            thresholds: [
                                skipped(failureThreshold: '0'),
                                failed(failureThreshold: '0')
                            ],
                            tools: [CTest(
                                pattern: 'Testing/**/*.xml',
                                deleteOutputFiles: true,
                                failIfNotNew: false,
                                skipNoTestFiles: true,
                                stopProcessingIfError: true
                            )]
                        )
                        deleteDir()
                    }
                }
            }
        }
    }
}