#include "GitRepoChangedCondition.hpp"


#include "trigger/context/ConditionContext.hpp"

#include <QProcess>
#include <QDebug>
#include <QStringList>


GitRepoChangedCondition::GitRepoChangedCondition()
	: ConditionBase("Git repo [git-repo] has changed.")
{
}

bool GitRepoChangedCondition::actualEvaluate(ConditionContext &conditionContext)
{
	Q_UNUSED(conditionContext);
	
	// Retrieve the remote URL from our phrase parameter.
	QString repoUrl = phrase()->parameterValue("git-repo").toString().trimmed();
	if (repoUrl.isEmpty()) {
		qWarning() << "GitRepoChangedCondition: [git-repo] parameter (remote URL) not set.";
		return false;
	}
	
	// 1. Check that `git` is available by calling `git --version`.
	{
		QProcess checkGit;
		checkGit.start(QStringLiteral("git"), { QStringLiteral("--version") });
		if (!checkGit.waitForStarted() || !checkGit.waitForFinished()) {
			qWarning() << "GitRepoChangedCondition: Could not start git process.";
			return false;
		}
		if (checkGit.exitCode() != 0) {
			qWarning() << "GitRepoChangedCondition: `git --version` failed. Exit code:"
					   << checkGit.exitCode() << checkGit.readAllStandardError();
			return false;
		}
	}
	
	// 2. Run `git ls-remote <repoUrl> HEAD` to get the latest remote HEAD commit.
	QProcess lsRemoteProc;
	lsRemoteProc.start(QStringLiteral("git"), { QStringLiteral("ls-remote"), repoUrl, QStringLiteral("HEAD") });
	if (!lsRemoteProc.waitForStarted() || !lsRemoteProc.waitForFinished()) {
		qWarning() << "GitRepoChangedCondition: git ls-remote failed to start/finish for" << repoUrl;
		return false;
	}
	if (lsRemoteProc.exitCode() != 0) {
		qWarning() << "GitRepoChangedCondition: git ls-remote returned error code"
				   << lsRemoteProc.exitCode() << lsRemoteProc.readAllStandardError();
		return false;
	}
	
	// Example output of `git ls-remote <repoUrl> HEAD`:
	//  "a3e8be1f0b9b9a3143a9fd2be8410ed3f2342f1c\tHEAD\n"
	QString output = QString::fromUtf8(lsRemoteProc.readAllStandardOutput()).trimmed();
	if (output.isEmpty()) {
		qWarning() << "GitRepoChangedCondition: ls-remote returned no output for" << repoUrl;
		return false;
	}
	
	// Typically the line is "<commit>\tHEAD"
	QStringList lines = output.split('\n', Qt::SkipEmptyParts);
	if (lines.isEmpty()) {
		qWarning() << "GitRepoChangedCondition: Could not parse ls-remote output for" << repoUrl;
		return false;
	}
	
	// We'll just use the first line in case there's only one HEAD ref.
	QString line = lines.first();
	QStringList tokens = line.split('\t');
	if (tokens.isEmpty()) {
		qWarning() << "GitRepoChangedCondition: Invalid ls-remote line:" << line;
		return false;
	}
	
	QString remoteHeadCommit = tokens.first().trimmed(); // the commit hash
	
	// 3. Compare the remote HEAD to the last known HEAD we stored.
	if (mLastRemoteHead.isEmpty()) {
		// If we have never seen a HEAD, store it and return false the first time.
		mLastRemoteHead = remoteHeadCommit;
		qDebug() << "GitRepoChangedCondition: first run, storing remote HEAD = " << remoteHeadCommit;
		return false;
	}
	
	bool changed = (remoteHeadCommit != mLastRemoteHead);
	if (changed) {
		qDebug() << "GitRepoChangedCondition: detected remote HEAD changed from" << mLastRemoteHead << "to" << remoteHeadCommit;
		mLastRemoteHead = remoteHeadCommit;
	}
	else {
		qDebug() << "GitRepoChangedCondition: no change, remote HEAD =" << remoteHeadCommit;
	}
	return changed;
}
