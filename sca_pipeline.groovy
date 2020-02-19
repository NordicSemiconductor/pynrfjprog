// This is a Jenkins pipeline file used to perform a static code analysis and automatic tagging with a new version
// number ( in x.y.z notation) when a merge to a certain branch is made. Automatic tagging increase only last digit (z)
// in the new version number. When you want to change first (x) or second (y) digit in the new version number you have
// to use ALTERNATIVE_VERSION parameter in Jenkins job.
// For the status to be visible on GitHub, you have to add "seedlabs-ateam" collaborator & add "Jenkins (Git plugin)"
// service to your git repository.
// During the first launch, you have to enter ALTERNATIVE_VERSION parameter.


@Library('JenkinsMain@2.16.15')_


pipelinePythonSCA(
    baseBranch: "fix_problem_with_concurrent_access_via_multiapi",
    agentLabel: "pylint",
    pythonVersion: '3.6',
    installFromSetup: true,
    runPipCheck: true,
    runUnitTests: false,
)